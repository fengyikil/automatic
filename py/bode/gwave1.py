import sys
import struct
import serial
import serial.tools.list_ports
import threading
import time
import collections
from PyQt5.QtCore import QObject, pyqtSignal, QTimer, Qt
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                             QHBoxLayout, QGroupBox, QLabel, QComboBox,
                             QPushButton, QMessageBox)
import pyqtgraph as pg

FRAME_HEAD = b'\xAA\xFF'
MAX_POINTS = 5000
REFRESH_MS = 100

# -------------------------------------------------


class Worker(QObject):
    data_ready = pyqtSignal(list, list, list)

    def __init__(self, port, baud):
        super().__init__()
        self.port, self.baud = port, baud
        self.running = True
        self.paused = False
        self.t_buf = collections.deque(maxlen=MAX_POINTS)
        self.u_buf = collections.deque(maxlen=MAX_POINTS)
        self.y_buf = collections.deque(maxlen=MAX_POINTS)

    def start(self):
        threading.Thread(target=self.run, daemon=True).start()

    def run(self):
        with serial.Serial(self.port, self.baud, timeout=0.1) as ser:
            buf = bytearray()
            while self.running:
                if ser.in_waiting:
                    buf.extend(ser.read(ser.in_waiting))
                self._parse(buf)
                time.sleep(0.002)

    def _parse(self, buf: bytearray):
        while True:
            try:
                idx = buf.index(FRAME_HEAD)
            except ValueError:
                return
            buf[:] = buf[idx:]
            if len(buf) < 18:
                return
            data_len = buf[3]
            if data_len != 12:
                buf.pop(0)
                continue
            payload = buf[:16]
            sum_rx, add_rx = buf[16], buf[17]
            s = a = 0
            for b in payload:
                s += b
                a += s
            if (s & 0xFF) != sum_rx or (a & 0xFF) != add_rx:
                buf.pop(0)
                continue
            t, u, y = struct.unpack('<fff', buf[4:16])
            if not self.paused:
                self.t_buf.append(t)
                self.u_buf.append(u)
                self.y_buf.append(y)
            buf[:] = buf[18:]

    def refresh(self):
        if self.t_buf:
            self.data_ready.emit(list(self.t_buf),
                                 list(self.u_buf),
                                 list(self.y_buf))

    def stop(self):
        self.running = False

# -------------------------------------------------


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle('串口波形实时显示（完整不卡死+独立缩放）')
        self.resize(1000, 700)

        # 串口配置
        gb_serial = QGroupBox('串口配置')
        lay = QHBoxLayout(gb_serial)
        self.cb_port = QComboBox()
        self.cb_port.addItems([p.device for p in serial.tools.list_ports.comports()])
        self.cb_baud = QComboBox()
        self.cb_baud.addItems(['9600', '57600', '115200', '921600', '460800'])
        self.cb_baud.setCurrentText('921600')
        self.btn_conn = QPushButton('连接')
        self.btn_conn.clicked.connect(self.toggle_connect)
        for w in (QLabel('串口:'), self.cb_port,
                  QLabel('波特率:'), self.cb_baud, self.btn_conn):
            lay.addWidget(w)

        # 波形图
        self.plot = pg.PlotWidget()
        self.plot.setLabel('left', 'Value')
        self.plot.setLabel('bottom', 'Time (s)')
        self.plot.addLegend()
        self.curve_u = self.plot.plot(pen='r', name='u (input)')
        self.curve_y = self.plot.plot(pen='g', name='y (output)')
        # 抗卡死
        self.plot.setDownsampling(True)
        self.plot.setClipToView(True)
        # 独立轴缩放
        self.plot.setMouseEnabled(x=True, y=False)
        # 事件过滤器：Ctrl+滚轮 → Y 轴，普通滚轮 → X 轴

        def wheelEventFilter(ev):
            if ev.modifiers() & Qt.ControlModifier:
                self.plot.setMouseEnabled(x=False, y=True)
                self.plot.wheelEvent(ev)
                self.plot.setMouseEnabled(x=True, y=False)
            else:
                self.plot.setMouseEnabled(x=True, y=False)
                self.plot.wheelEvent(ev)
            ev.accept()
        self.plot.wheelEvent = wheelEventFilter

        # 控制按钮
        self.btn_pause = QPushButton('暂停/继续')
        self.btn_pause.setCheckable(True)
        self.btn_pause.toggled.connect(self.toggle_pause)
        self.btn_clear = QPushButton('清屏')
        self.btn_clear.clicked.connect(self.clear_plot)

        # 主布局
        vbox = QVBoxLayout()
        vbox.addWidget(gb_serial)
        vbox.addWidget(self.plot, stretch=1)
        vbox.addWidget(self.btn_pause)
        vbox.addWidget(self.btn_clear)
        central = QWidget()
        central.setLayout(vbox)
        self.setCentralWidget(vbox)

        self.worker = None
        self.timer = QTimer()
        self.timer.timeout.connect(self.refresh_curves)

    # -------------------------------------------------
    def toggle_connect(self):
        if self.worker is None:
            port = self.cb_port.currentText()
            baud = int(self.cb_baud.currentText())
            try:
                self.worker = Worker(port, baud)
                self.worker.data_ready.connect(self.update_curves)
                self.worker.start()
                self.timer.start(REFRESH_MS)
            except Exception as e:
                QMessageBox.critical(self, '串口错误', str(e))
                return
            self.btn_conn.setText('断开')
            self.cb_port.setEnabled(False)
            self.cb_baud.setEnabled(False)
        else:
            self.worker.stop()
            self.timer.stop()
            self.worker = None
            self.btn_conn.setText('连接')
            self.cb_port.setEnabled(True)
            self.cb_baud.setEnabled(True)

    def update_curves(self, t, u, y):
        self.curve_u.setData(t, u)
        self.curve_y.setData(t, y)

    def refresh_curves(self):
        if self.worker:
            self.worker.refresh()

    def toggle_pause(self, checked):
        if self.worker:
            self.worker.paused = checked

    def clear_plot(self):
        if self.worker:
            self.worker.t_buf.clear()
            self.worker.u_buf.clear()
            self.worker.y_buf.clear()
        self.curve_u.clear()
        self.curve_y.clear()

    def closeEvent(self, e):
        if self.worker:
            self.worker.stop()
        super().closeEvent(e)


# -------------------------------------------------
if __name__ == '__main__':
    app = QApplication(sys.argv)
    w = MainWindow()
    w.show()
    sys.exit(app.exec_())

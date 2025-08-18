import sys
import time
import struct
import serial
import serial.tools.list_ports
from PyQt5.QtCore import QThread, pyqtSignal
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                             QHBoxLayout, QGroupBox, QLabel, QComboBox,
                             QPushButton, QMessageBox)
import pyqtgraph as pg

# -------------------- 协议常量 --------------------
FRAME_HEAD = b'\xAA\xFF'   # 固定帧头
FRAME_ID = 0xF1          # 帧 ID（FUNC ID）
# --------------------------------------------------
DEBUG_RAW = True      # 打开/关闭原始字节打印
DEBUG_CB = True      # 打开/关闭解码结果打印


class SerialReader(QThread):
    new_data = pyqtSignal(float, float, float)

    def __init__(self, port, baud):
        super().__init__()
        self.port, self.baud = port, baud
        self.running = True

    def run(self):
        ser = serial.Serial(self.port, self.baud, timeout=0.1)
        buf = bytearray()
        while self.running:
            if ser.in_waiting:
                buf.extend(ser.read(ser.in_waiting))
            self._parse(buf)
            time.sleep(0.002)
        ser.close()

    def _parse(self, buf: bytearray):
        while True:
            try:
                idx = buf.index(b'\xAA\xFF')
            except ValueError:
                return
            buf[:] = buf[idx:]
            if len(buf) < 18:
                return
            data_len = buf[3]
            if data_len != 12:
                buf.pop(0)
                continue

            # 与下位机一致的 16 字节校验
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
            self.new_data.emit(t, u, y)
            buf[:] = buf[18:]

    def stop(self):
        self.running = False
        self.wait()


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle('串口波形实时显示')
        self.resize(1000, 700)

        # ---------------- 串口配置区 ----------------
        gb_serial = QGroupBox('串口配置')
        lay_serial = QHBoxLayout(gb_serial)
        lay_serial.addWidget(QLabel('串口:'))
        self.cb_port = QComboBox()
        self.cb_port.addItems([p.device for p in serial.tools.list_ports.comports()])
        lay_serial.addWidget(self.cb_port)

        lay_serial.addWidget(QLabel('波特率:'))
        self.cb_baud = QComboBox()
        self.cb_baud.addItems(['9600', '57600', '115200', '921600', '460800'])
        self.cb_baud.setCurrentText('921600')
        lay_serial.addWidget(self.cb_baud)

        self.btn_conn = QPushButton('连接')
        self.btn_conn.clicked.connect(self.toggle_connect)
        lay_serial.addWidget(self.btn_conn)

        # ---------------- 波形图 ----------------
        self.plot = pg.PlotWidget()
        self.plot.setLabel('left', 'Value')
        self.plot.setLabel('bottom', 'Time (s)')
        self.plot.addLegend()
        self.curve_u = self.plot.plot(pen='r', name='u (input)')
        self.curve_y = self.plot.plot(pen='g', name='y (output)')

        # ---------------- 控制按钮 ----------------
        self.btn_pause = QPushButton('暂停/继续')
        self.btn_pause.setCheckable(True)
        self.btn_pause.toggled.connect(self.toggle_pause)
        self.btn_clear = QPushButton('清屏')
        self.btn_clear.clicked.connect(self.clear_plot)

        # ---------------- 主布局 ----------------
        vbox = QVBoxLayout()
        vbox.addWidget(gb_serial)
        vbox.addWidget(self.plot, stretch=1)
        vbox.addWidget(self.btn_pause)
        vbox.addWidget(self.btn_clear)

        central = QWidget()
        central.setLayout(vbox)
        self.setCentralWidget(central)

        # ---------------- 数据缓存 ----------------
        self.t_buf, self.u_buf, self.y_buf = [], [], []
        self.paused = False
        self.thread = None

    # 连接 / 断开
    def toggle_connect(self):
        if self.thread is None:
            port = self.cb_port.currentText()
            baud = int(self.cb_baud.currentText())
            try:
                self.thread = SerialReader(port, baud)
                self.thread.new_data.connect(self.update_plot)
                self.thread.start()
            except serial.SerialException as e:
                QMessageBox.critical(self, '串口错误', str(e))
                return
            self.btn_conn.setText('断开')
            self.cb_port.setEnabled(False)
            self.cb_baud.setEnabled(False)
        else:
            self.thread.stop()
            self.thread = None
            self.btn_conn.setText('连接')
            self.cb_port.setEnabled(True)
            self.cb_baud.setEnabled(True)

    # 更新曲线
    def update_plot(self, t, u, y):
        self.t_buf.append(t)
        self.u_buf.append(u)
        self.y_buf.append(y)
        if len(self.t_buf) > 10000:
            self.t_buf = self.t_buf[-10000:]
            self.u_buf = self.u_buf[-10000:]
            self.y_buf = self.y_buf[-10000:]
        self.curve_u.setData(self.t_buf, self.u_buf)
        self.curve_y.setData(self.t_buf, self.y_buf)

    def toggle_pause(self, checked):
        self.paused = checked

    def clear_plot(self):
        self.t_buf.clear()
        self.u_buf.clear()
        self.y_buf.clear()
        self.curve_u.clear()
        self.curve_y.clear()

    # 关闭窗口时停止线程
    def closeEvent(self, event):
        if self.thread:
            self.thread.stop()
        super().closeEvent(event)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    w = MainWindow()
    w.show()
    sys.exit(app.exec_())

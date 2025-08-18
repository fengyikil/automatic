import serial
import struct
import serial.tools.list_ports

PORT = 'COM7'          # 改成你的
BAUD = 921600


def decode_once(buf: bytearray):
    """返回 (t,u,y) 或 None"""
    while True:
        try:
            idx = buf.index(b'\xAA\xFF')
        except ValueError:
            return None
        buf[:] = buf[idx:]
        if len(buf) < 18:
            return None
        data_len = buf[3]
        if data_len != 12:          # 长度不对
            buf.pop(0)
            continue
        payload = buf[:16]          # 前 16 字节用于校验
        sum_rx, add_rx = buf[16], buf[17]

        s = a = 0
        for b in payload:
            s += b
            a += s
        if (s & 0xFF) == sum_rx and (a & 0xFF) == add_rx:
            t, u, y = struct.unpack('<fff', buf[4:16])
            del buf[:18]
            return t, u, y
        buf.pop(0)


with serial.Serial(PORT, BAUD, timeout=0.1) as ser:
    print('打开', ser.port, ser.baudrate)
    buf = bytearray()
    while True:
        if ser.in_waiting:
            buf.extend(ser.read(ser.in_waiting))
        res = decode_once(buf)
        if res:
            t, u, y = res
            print(f'{t:8.3f}  {u:8.3f}  {y:8.3f}')

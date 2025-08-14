import serial
import struct
import time
import math


class AnonymizerV725:
    def __init__(self, port, baudrate=115200, target_addr=0xFF):
        """
        初始化匿名上位机V7.2.5通信协议
        :param port: 串口号(如'COM3'或'/dev/ttyUSB0')
        :param baudrate: 波特率(默认115200)
        :param target_addr: 目标地址(默认0xFF表示广播)
        """
        self.serial_port = None
        self.port = port
        self.baudrate = baudrate
        self.target_addr = target_addr
        self.connect()

    def connect(self):
        """连接串口"""
        try:
            self.serial_port = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                bytesize=8,
                parity='N',
                stopbits=1,
                timeout=1
            )
            print(f"已连接 {self.port} @ {self.baudrate}bps")
            return True
        except Exception as e:
            print(f"连接串口失败: {e}")
            return False

    def disconnect(self):
        """断开串口连接"""
        if self.serial_port and self.serial_port.is_open:
            self.serial_port.close()
            print("串口连接已关闭")

    def _calculate_checksums(self, data):
        """
        计算校验和(SUM CHECK和ADD CHECK)
        :param data: 从帧头开始的数据(包括帧头)
        :return: (sum_check, add_check)
        """
        sum_check = 0
        add_check = 0
        for byte in data:
            sum_check = (sum_check + byte) & 0xFF
            add_check = (add_check + sum_check) & 0xFF
        return sum_check, add_check

    def send_frame(self, frame_id, data):
        """
        发送协议帧(严格遵循匿名协议V7.2.5格式)
        :param frame_id: 功能码(0x00-0xFF)
        :param data: 数据内容(字节数组)
        :return: 是否发送成功
        """
        if not self.serial_port or not self.serial_port.is_open:
            print("串口未连接")
            return False

        try:
            # 构建帧头部分
            frame = bytearray()
            frame.append(0xAA)  # 帧头HEAD
            frame.append(self.target_addr)  # 目标地址D_ADDR
            frame.append(frame_id)  # 功能码ID
            frame.append(len(data))  # 数据长度LEN

            # 添加数据内容
            frame.extend(data)

            # 计算校验和(从帧头开始计算)
            sum_check, add_check = self._calculate_checksums(frame)

            # 添加校验和
            frame.append(sum_check)  # SUM CHECK
            frame.append(add_check)  # ADD CHECK

            # 发送完整帧
            self.serial_port.write(frame)
            self.serial_port.flush()

            # 打印调试信息
            # # print(f"[发送帧] {len(frame)}字节: {frame.hex(' ').upper()}")
            # print(f"帧结构: HEAD[{frame[0]:02X}] D_ADDR[{frame[1]:02X}] ID[{frame[2]:02X}] "
            #       f"LEN[{frame[3]:02X}] DATA[{frame[4:-2].hex(' ').upper()}] "
            #       f"SC[{frame[-2]:02X}] AC[{frame[-1]:02X}]")
            return True
        except Exception as e:
            print(f"发送数据帧失败: {e}")
            return False

    def send_flexible_frame_4byte(self, frame_id, *values):
        """
        发送灵活格式帧(0xF1-0xFA)，每个数据强制4字节
        :param frame_id: 帧ID(0xF1-0xFA)
        :param values: 要发送的数据值(自动转换为4字节格式)
        """
        if frame_id < 0xF1 or frame_id > 0xFA:
            print("错误: 灵活格式帧ID必须在0xF1-0xFA范围内")
            return False

        data = bytearray()
        for value in values:
            if isinstance(value, float):
                # 浮点数转换为int32(放大1000倍)
                data.extend(struct.pack('<f', float(value)))
            elif isinstance(value, int):
                # 整数强制转换为int32
                data.extend(struct.pack('<i', value))
            elif isinstance(value, bytes) and len(value) == 4:
                # 直接添加4字节数据
                data.extend(value)
            else:
                print(f"错误: 不支持的数据类型或长度 {type(value)}")
                return False

        return self.send_frame(frame_id, data)

    def send_raw_bytes(self, frame_id, byte_data):
        """
        发送原始字节数据帧
        :param frame_id: 帧ID
        :param byte_data: 字节数据(bytearray或bytes)
        """
        if not isinstance(byte_data, (bytes, bytearray)):
            print("错误: 数据必须是bytes或bytearray类型")
            return False

        return self.send_frame(frame_id, byte_data)

    def send_float_as_int32(self, frame_id, float_values, scale=1000):
        """
        发送浮点数据(转换为int32)
        :param frame_id: 帧ID
        :param float_values: 浮点数列表
        :param scale: 放大倍数(默认1000)
        """
        data = bytearray()
        for value in float_values:
            # 浮点数转换为int32
            data.extend(struct.pack('<i', int(value * scale)))
        return self.send_frame(frame_id, data)

    def send_ints_as_int32(self, frame_id, int_values):
        """
        发送整数数据(转换为int32)
        :param frame_id: 帧ID
        :param int_values: 整数列表
        """
        data = bytearray()
        for value in int_values:
            # 整数转换为int32
            data.extend(struct.pack('<i', value))
        return self.send_frame(frame_id, data)

def busy_wait_us(us):
    end = time.perf_counter_ns() + us * 1000
    while time.perf_counter_ns() < end:
        pass

# 使用示例
if __name__ == "__main__":
    # 创建通信对象 - 替换为你的实际串口号
    ano = AnonymizerV725(port='COM11', baudrate=921600)
    count  = 1
    if ano.serial_port and ano.serial_port.is_open:
        while (1):
            # 示例1: 发送灵活格式帧(每个数据4字节)
            # print("\n发送灵活格式帧(4字节数据)...")
            ano.send_flexible_frame_4byte(0xF1, count, -count, math.sin(2* count * 0.001 * math.pi))
            count = count + 1
            # 示例2: 发送原始字节数据
            # print("\n发送原始字节数据...")
            # raw_data = bytes.fromhex("01 02 03 04 AA")
            # ano.send_raw_bytes(0xF1, raw_data)

            # # 示例3: 发送浮点数据(转换为int32)
            # print("\n发送浮点数据...")
            # ano.send_float_as_int32(0xF2, [1.234, -5.678, 9.999])

            # # 示例4: 发送整数数据(转换为int32)
            # print("\n发送整数数据...")
            # ano.send_ints_as_int32(0xF3, [1000000, -2000000, 3000000])

            # 等待数据发送完成
            # time.sleep(0.01)
            busy_wait_us(1000)
            # print("\n用户中断")
            # 断开连接
            # ano.disconnect()
    else:
        print("无法连接到串口，请检查连接")

/* anonymizer_v725.h ********************************************/
#ifndef __ANONYMIZER_V725_H
#define __ANONYMIZER_V725_H

#include <stdint.h>
 /* 发送一帧完整数据（内部完成校验和计算） */
/* 目标地址，0xFF=广播  功能码 0x00~0xFF  * 数据负载      数据长度 */
int ano_send_frame(uint8_t target_addr, uint8_t frame_id, const uint8_t* data, uint16_t data_len);
/* 快捷函数：把 1~N 个 float 按 V7.2.5 灵活帧格式（4 字节/值）发送 */
int ano_send_float(uint8_t target_addr, uint8_t frame_id, const float* values, uint8_t cnt);
float chirp_test(float fs, float dur, float f0, float f1, float y);
#endif /* __ANONYMIZER_V725_H */
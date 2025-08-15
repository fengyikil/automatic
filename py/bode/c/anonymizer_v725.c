/* anonymizer_v725.c *******************************************/
#include "anonymizer_v725.h"
#include <string.h>
#include "fy_protocol_hal.h"
#include "math.h"
/* 计算 SUM_CHECK 与 ADD_CHECK */
static inline void calc_checksum(const uint8_t* buf, uint16_t len, uint8_t* sum_chk, uint8_t* add_chk)
{
    uint8_t s = 0, a = 0;
    for (uint16_t i = 0; i < len; ++i) {
        s += buf[i];
        a += s;
    }
    *sum_chk = s;
    *add_chk = a;
}

int ano_send_frame(uint8_t target_addr, uint8_t frame_id, const uint8_t* data, uint16_t data_len)
{
    /* 固定帧头 4 字节 + 数据 + 2 字节校验 */
    uint16_t frame_len = 4 + data_len + 2;
    uint8_t frame[256]; /* 若需更长可动态申请 */

    if (frame_len > sizeof(frame)) /* 简单保护 */
        return -1;

    frame[0] = 0xAA;        /* HEAD  */
    frame[1] = target_addr; /* D_ADDR*/
    frame[2] = frame_id;    /* ID    */
    frame[3] = data_len;    /* LEN   */

    if (data && data_len)
        memcpy(&frame[4], data, data_len);

    uint8_t sum_chk, add_chk;
    calc_checksum(frame, 4 + data_len, &sum_chk, &add_chk);

    frame[4 + data_len] = sum_chk;
    frame[4 + data_len + 1] = add_chk;

    /* 调用底层驱动一次性发出 */
    fy_com_write_buf(COM_UART2, frame, frame_len);
    return 0;
}

/* 直接按 IEEE-754 小端发送浮点数组 */
int ano_send_float(uint8_t target_addr, uint8_t frame_id, const float* values, uint8_t cnt)
{
    if (cnt == 0)
        return -1;

    union {
        float f;
        uint8_t u[4];
    } tmp;
    uint8_t buf[256];
    if (cnt * 4 > sizeof(buf))
        return -1;

    for (uint8_t i = 0; i < cnt; ++i) {
        tmp.f = values[i];
        buf[4 * i + 0] = tmp.u[0];
        buf[4 * i + 1] = tmp.u[1];
        buf[4 * i + 2] = tmp.u[2];
        buf[4 * i + 3] = tmp.u[3];
    }
    return ano_send_frame(target_addr, frame_id, buf, cnt * 4);
}

float chirp_test(float fs, float dur, float f0, float f1, float y)
{
#define M_PI (3.1415926f)

    static float phase = 0.0f;
    static uint32_t k = 0;  // 已调用次数，用作时间索引
    float u = 0;
    if (k >= (uint32_t)(dur * fs)) {  // 扫频结束
        k = 0;
        return 0.0f;
    }

    float t = k / fs;

    // float f = f0 + (f1 - f0) * (t / dur); /* 线性扫频：f(t) = f0 + (f1-f0)*t/T */

    float f = f0 * powf(f1 / f0, t / dur); /* 对数扫频：f(t) = f0 * (f1/f0)^(t/T) */

    phase += 2.0f * (float)M_PI * f / fs;
    k++;
    u = sinf(phase);
    float vec[3] = {k / fs, u, y};
    ano_send_frame(0xff, 0xF1, vec, 12);
    return u;
}
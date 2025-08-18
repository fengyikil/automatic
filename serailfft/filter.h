/*
 * @Description: In User Settings Edit
 * @Author: MJY
 * @Date: 2019-08-29 09:10:28
 * @LastEditTime: 2019-08-29 10:22:29
 * @LastEditors: Please set LastEditors
 */
#ifndef _FILTER_H_
#define _FILTER_H_
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <stdint.h>

typedef struct
{
    float f;
    float wn_hz;
    float wn_hz_1;

    float K; //权重
    float Cn_1; //上次的输出
}LPF_1OrdType;

typedef struct
{
    float f;
    float wn_hz;
    float wn_hz_1;
    float zeta;
    float zeta_1;

    float A;  //权重
    float B1;
    float B2;

    float Cn_1; //旧的输出
    float Cn_2;
}LPF_2OrdType;

typedef struct
{
    float sample_freq;
    float notch_freq;
    float notch_freq1;
    float bandwidth;
    float bandwidth1;

    float a1;
    float a2;
    float b0;
    float b1;
    float b2;

    float delay_element_1;
    float delay_element_2;
    float delay_element_output_1;
    float delay_element_output_2;
}Notch_Filter_Type;

typedef struct pt3Filter_s {
    float state;
    float state1;
    float state2;
    float k;
} pt3Filter_t;

/* this holds the data required to update samples thru a filter */
typedef struct biquadFilter_s {
    float refreshRate;
    float filterFreq;
    float filterFreq1;
    float b0, b1, b2, a1, a2;
    float x1, x2, y1, y2;
} biquadFilter_t;

    typedef enum {
        FILTER_LPF,    // 2nd order Butterworth section
        FILTER_NOTCH,
        FILTER_BPF,
        } biquadFilterType_e;

/* 1阶低通滤波 */
void LPF_1Ord_Init(float f, float wn_hz, LPF_1OrdType *lpf);
float LPF_1Ord_Cal(float Rn, LPF_1OrdType *lpf);

/* 2阶低通滤波 */
void LPF_2Ord_Init(float f, float wn_hz, float zeta, LPF_2OrdType *lpf);
float LPF_2Ord_Cal(float Rn, LPF_2OrdType *lpf);

/* 带阻滤波 */
void NOTCH_Filter_Init(float f, float notch_freq, float bandwidth, Notch_Filter_Type *lpf);
float NOTCH_Filter_Cal(float Rn, Notch_Filter_Type *lpf);

float pt3FilterGain(float f_cut, float dT);
void pt3FilterInit(pt3Filter_t *filter, float k);
void pt3FilterUpdateCutoff(pt3Filter_t *filter, float k);
float pt3FilterApply(pt3Filter_t *filter, float input);

void biquadFilterInitLPF(biquadFilter_t *filter, float filterFreq, uint32_t refreshRate);
void biquadFilterInit(biquadFilter_t *filter, float filterFreq, uint32_t refreshRate, float Q, biquadFilterType_e filterType);
void biquadFilterUpdate(biquadFilter_t *filter, float filterFreq, uint32_t refreshRate, float Q, biquadFilterType_e filterType);
void biquadFilterUpdateLPF(biquadFilter_t *filter, float filterFreq, uint32_t refreshRate);
float biquadFilterApplyDF1(biquadFilter_t *filter, float input);
float biquadFilterApply(biquadFilter_t *filter, float input);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif

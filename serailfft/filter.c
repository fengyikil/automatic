/*
 * @Description: In User Settings Edit
 * @Author: MJY
 * @Date: 2019-08-29 09:10:17
 * @LastEditTime: 2019-08-29 10:22:52
 * @LastEditors: Please set LastEditors
 */

#include "filter.h"
#include "math.h"

#define PI   (3.1416f)

/**
 * @description: 2阶低通滤波器权重初始化
 * @param
 *          f : 计算频率
 *          wn_hz : 低通滤波转折频率
 *          lpf   : 低通滤波结构体
 * @return:
 *          none
 */
void LPF_1Ord_Init(float f, float wn_hz, LPF_1OrdType *lpf)
{
    float wn_rad = 2*PI*wn_hz;
    float T = 1.0f/f;
    lpf->K = expf(-wn_rad*T);
}

/**
 * @description: 1阶低通滤波器计算
 * @param
 *          Rn  : 当前滤波器的输入
 *          lpf : 低通滤波结构体
 * @return:
 *          Cn : 当前滤波器的输出
 */
float LPF_1Ord_Cal(float Rn, LPF_1OrdType *lpf)
{
    if(lpf->wn_hz != lpf->wn_hz_1)
    {
        LPF_1Ord_Init(lpf->f, lpf->wn_hz, lpf);
        lpf->wn_hz_1 = lpf->wn_hz;
    }

    float Cn;
    Cn = Rn*(1-lpf->K) +  lpf->Cn_1*lpf->K;
    lpf->Cn_1 = Cn;

    return Cn;
}

/**
 * @description: 2阶低通滤波器权重初始化
 * @param
 *          f : 计算频率
 *          wn_hz : 低通滤波转折频率
 *          zeta  : 低通滤波阻尼系数  0 < zeta < 1
 *          lpf   : 低通滤波结构体
 * @return:
 *          none
 */
void LPF_2Ord_Init(float f, float wn_hz, float zeta, LPF_2OrdType *lpf)
{
    lpf->f = f;
    lpf->wn_hz = wn_hz;
    lpf->wn_hz_1 = wn_hz;
    lpf->zeta  = zeta;
    lpf->zeta_1  = zeta;

    float wn_rad = 2*PI*wn_hz;
    float T = 1.0f/f;
    lpf->B1 = -2*expf(-zeta*wn_rad*T)*cosf(wn_rad*T*sqrtf(1 - zeta*zeta));
    lpf->B2 = exp(-2*zeta*wn_rad*T);
    lpf->A  = 1 + lpf->B1 + lpf->B2;
}

/**
 * @description: 2阶低通滤波器计算
 * @param


*          Rn  : 当前滤波器的输入
          *          lpf : 低通滤波结构体
                     * @return:
 *          Cn : 当前滤波器的输出
    */
    float LPF_2Ord_Cal(float Rn, LPF_2OrdType *lpf)
{
    if(lpf->wn_hz != lpf->wn_hz_1 || lpf->zeta != lpf->zeta_1)
    {
        LPF_2Ord_Init(lpf->f, lpf->wn_hz, lpf->zeta, lpf);
        lpf->wn_hz_1 = lpf->wn_hz;
        lpf->zeta_1 = lpf->zeta;
    }

    float Cn;
    Cn = lpf->A*Rn - lpf->B1*lpf->Cn_1 - lpf->B2*lpf->Cn_2;

    lpf->Cn_1 = Cn;
    lpf->Cn_2 = lpf->Cn_1;

    return Cn;
}

void NOTCH_Filter_Init(float f, float notch_freq, float bandwidth, Notch_Filter_Type *lpf)
{
    lpf->sample_freq = f;
    lpf->notch_freq  = notch_freq;
    lpf->notch_freq1 = notch_freq;
    lpf->bandwidth  = bandwidth;
    lpf->bandwidth1 = bandwidth;


    const float alpha = tanf(PI * bandwidth / f);
    const float beta = -cosf(2.0f * PI * notch_freq / f);
    const float a0_inv = 1.0f / (alpha + 1.0f);

    lpf->b0 = a0_inv;
    lpf->b1 = 2.0f * beta * a0_inv;
    lpf->b2 = a0_inv;

    lpf->a1 = lpf->b1;
    lpf->a2 = (1.0f - alpha) * a0_inv;

    lpf->delay_element_1 = 0.0f;
    lpf->delay_element_2 = 0.0f;
    lpf->delay_element_output_1 = 0.0f;
    lpf->delay_element_output_2 = 0.0f;
}

float NOTCH_Filter_Cal(float Rn, Notch_Filter_Type *lpf)
{
    if (lpf->notch_freq != lpf->notch_freq1||
        lpf->bandwidth  != lpf->bandwidth1)
    {
        NOTCH_Filter_Init(lpf->sample_freq, lpf->notch_freq, lpf->bandwidth, lpf);
    }

    // Direct Form I implementation
    float output = lpf->b0 * Rn + lpf->b1 * lpf->delay_element_1 + lpf->b2 * lpf->delay_element_2 - lpf->a1 * lpf->delay_element_output_1 - lpf->a2 * lpf->delay_element_output_2;

    // shift inputs
    lpf->delay_element_2 = lpf->delay_element_1;
    lpf->delay_element_1 = Rn;

    // shift outputs
    lpf->delay_element_output_2 = lpf->delay_element_output_1;
    lpf->delay_element_output_1 = output;

    return output;
}

// PT3 Low Pass filter
float pt3FilterGain(float f_cut, float dT)
{
    const float order = 3.0f;
    const float orderCutoffCorrection = 1 / sqrtf(powf(2, 1.0f / order) - 1);
    float RC = 1 / (2 * orderCutoffCorrection * PI * f_cut);
    // float RC = 1 / (2 * 1.961459177f * M_PIf * f_cut);
    // where 1.961459177 = 1 / sqrt( (2^(1 / order) - 1) ) and order is 3
    return dT / (RC + dT);
}

void pt3FilterInit(pt3Filter_t *filter, float k)
{
    filter->state  = 0.0f;
    filter->state1 = 0.0f;
    filter->state2 = 0.0f;
    filter->k = k;
}

void pt3FilterUpdateCutoff(pt3Filter_t *filter, float k)
{
    filter->k = k;
}

float pt3FilterApply(pt3Filter_t *filter, float input)
{
    filter->state1 = filter->state1 + filter->k * (input - filter->state1);
    filter->state2 = filter->state2 + filter->k * (filter->state1 - filter->state2);
    filter->state = filter->state + filter->k * (filter->state2 - filter->state);
    return filter->state;
}

#define BIQUAD_Q 1.0f / sqrtf(2.0f)     /* quality factor - 2nd order butterworth*/
#define sinPolyCoef3 -1.666568107e-1f
#define sinPolyCoef5  8.312366210e-3f
#define sinPolyCoef7 -1.849218155e-4f
#define sinPolyCoef9  0
/* sets up a biquad filter as a 2nd order butterworth LPF */
void biquadFilterInitLPF(biquadFilter_t *filter, float filterFreq, uint32_t refreshRate)
{
    biquadFilterInit(filter, filterFreq, refreshRate, BIQUAD_Q, FILTER_LPF);
}

void biquadFilterInit(biquadFilter_t *filter, float filterFreq, uint32_t refreshRate, float Q, biquadFilterType_e filterType)
{
    biquadFilterUpdate(filter, filterFreq, refreshRate, Q, filterType);

    // zero initial samples
    filter->x1 = filter->x2 = 0;
    filter->y1 = filter->y2 = 0;
}

float sin_approx(float x)
{
    int32_t xint = x;
    if (xint < -32 || xint > 32) return 0.0f;                               // Stop here on error input (5 * 360 Deg)
    while (x >  PI) x -= (2.0f * PI);                                 // always wrap input angle to -PI..PI
    while (x < -PI) x += (2.0f * PI);
    if (x >  (0.5f * PI)) x =  (0.5f * PI) - (x - (0.5f * PI));   // We just pick -90..+90 Degree
    else if (x < -(0.5f * PI)) x = -(0.5f * PI) - ((0.5f * PI) + x);
    float x2 = x * x;
    return x + x * x2 * (sinPolyCoef3 + x2 * (sinPolyCoef5 + x2 * (sinPolyCoef7 + x2 * sinPolyCoef9)));
}

float cos_approx(float x)
{
    return sin_approx(x + (0.5f * PI));
}

void biquadFilterUpdate(biquadFilter_t *filter, float filterFreq, uint32_t refreshRate, float Q, biquadFilterType_e filterType)
{
    // setup variables
    const float omega = 2.0f * PI * filterFreq * refreshRate * 0.000001f;
    const float sn = sin_approx(omega);
    const float cs = cos_approx(omega);
    const float alpha = sn / (2.0f * Q);

    switch (filterType) {
    case FILTER_LPF:
        // 2nd order Butterworth (with Q=1/sqrt(2)) / Butterworth biquad section with Q
        // described in http://www.ti.com/lit/an/slaa447/slaa447.pdf
        filter->b1 = 1 - cs;
        filter->b0 = filter->b1 * 0.5f;
        filter->b2 = filter->b0;
        filter->a1 = -2 * cs;
        filter->a2 = 1 - alpha;
        break;
    case FILTER_NOTCH:
        filter->b0 = 1;
        filter->b1 = -2 * cs;
        filter->b2 = 1;
        filter->a1 = filter->b1;
        filter->a2 = 1 - alpha;
        break;
    case FILTER_BPF:
        filter->b0 = alpha;
        filter->b1 = 0;
        filter->b2 = -alpha;
        filter->a1 = -2 * cs;
        filter->a2 = 1 - alpha;
        break;
    }

    const float a0 = 1 + alpha;

    // precompute the coefficients
    filter->b0 /= a0;
    filter->b1 /= a0;
    filter->b2 /= a0;
    filter->a1 /= a0;
    filter->a2 /= a0;
}

void biquadFilterUpdateLPF(biquadFilter_t *filter, float filterFreq, uint32_t refreshRate)
{
    filter->refreshRate = refreshRate;
    filter->filterFreq  = filterFreq;
    filter->filterFreq1 = filterFreq;
    biquadFilterUpdate(filter, filterFreq, refreshRate, BIQUAD_Q, FILTER_LPF);
}

/* Computes a biquadFilter_t filter on a sample (slightly less precise than df2 but works in dynamic mode) */
float biquadFilterApplyDF1(biquadFilter_t *filter, float input)
{
    /* compute result */
    const float result = filter->b0 * input + filter->b1 * filter->x1 + filter->b2 * filter->x2 - filter->a1 * filter->y1 - filter->a2 * filter->y2;

    /* shift x1 to x2, input to x1 */
    filter->x2 = filter->x1;
    filter->x1 = input;

    /* shift y1 to y2, result to y1 */
    filter->y2 = filter->y1;
    filter->y1 = result;

    return result;
}

/* Computes a biquadFilter_t filter in direct form 2 on a sample (higher precision but can't handle changes in coefficients */
float biquadFilterApply(biquadFilter_t *filter, float input)
{
    if (filter->filterFreq != filter->filterFreq1)
    {
        biquadFilterUpdateLPF(filter, filter->filterFreq, filter->refreshRate);
    }

    const float result = filter->b0 * input + filter->x1;
    filter->x1 = filter->b1 * input - filter->a1 * result + filter->x2;
    filter->x2 = filter->b2 * input - filter->a2 * result;
    return result;
}



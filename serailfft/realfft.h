#ifndef __REAL_FFT_H__
#define __REAL_FFT_H__
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define REAL_FFT_VOLUME 32

typedef struct
{
    float buf[REAL_FFT_VOLUME];
    int count;
    int pos;
    float value;
} FFtPv;
void fpv_fft_init();
void fpv_sample(FFtPv* fpv,float Xn);
void fpv_update(FFtPv* fpv);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __REAL_FFT_H__

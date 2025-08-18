#ifndef __MEAN_VARIANCE_H__
#define __MEAN_VARIANCE_H__
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define MEAN_VARIANCE_VOLUME 8
#define MEAN_VARIANCE_LEN (MEAN_VARIANCE_VOLUME+1)

typedef struct
{
    float buf[MEAN_VARIANCE_LEN];
    int front; //队头指针
    int rear; //队尾指针
    float mean;
    float variance;
} MeanVariance;

void mean_variance_update(MeanVariance* mt,float Xn);
void mean_variance_reset(MeanVariance* mt);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __MEAN_VARIANCE_H__

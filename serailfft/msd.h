#ifndef __MSD_H__
#define __MSD_H__
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct
{
    struct fifo* data;
    float mean;
    float variance;
} MsdType;

MsdType* msd_init(unsigned int size);
MsdType* msd_update(MsdType* mt,float Xn);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __FFT_H__

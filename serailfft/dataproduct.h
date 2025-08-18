#ifndef DATAPRODUCT_H
#define DATAPRODUCT_H

#include "fft.h"
#include "fifo.h"
#include "filter.h"

#define DATA_CHANNEL_NUM 3
#define DATA_GUI_CHANNEL_NUM 8
#define PER_SIZE    2048  //必须为2的指数


#pragma pack (1)
typedef struct name
{
    char head;
    float channel[DATA_CHANNEL_NUM];
}SampleData;
#pragma pack()


class DataProduct
{
public:
    DataProduct();
    void DataHandle(struct fifo* f);
    fft_config_t* real_fft_plan;
    LPF_2OrdType lpf_2nd_c_p;		//速度环P值滤

    float g_fft_in[PER_SIZE] = {0.0};
    float g_fft_out[PER_SIZE] = {0.0};
    struct fifo* fc[DATA_GUI_CHANNEL_NUM];

};

#endif // DATAPRODUCT_H

#include "dataproduct.h"
#include "string.h"
#include "math.h"
#include <stdio.h>

DataProduct::DataProduct()
{
    real_fft_plan = fft_init(PER_SIZE, FFT_REAL, FFT_FORWARD, g_fft_in, g_fft_out);
    LPF_2Ord_Init(2000.0f,100.0f, 0.7071f,  &lpf_2nd_c_p);
    for(int i = 0;i<DATA_GUI_CHANNEL_NUM;i++)
    {
        fc[i] = fifo_init(16384);
    }
}

static bool clean_head(fifo *f)
{
    char head;
    /* 简单清理一下,对齐数据 */
    while (true) {
        if(fifo_get_no_out(f,&head,1) != 1)
        {
            return false;
        }
        if(head != '+')
        {
            fifo_out(f,1);
        }
        else
        {
            return true;
        }
    }
}

/*
void DataProduct::DataHandle(fifo *f)
{

    if(!clean_head(f))
    {
        return;
    }

    SampleData src[PER_SIZE];
    float buf1[PER_SIZE] = {0};
    float buf2[PER_SIZE] = {0};

    float out1[PER_SIZE] = {0};
    float out2[PER_SIZE] = {0};

    if(fifo_get(f,src,PER_SIZE*sizeof(SampleData)) == PER_SIZE*sizeof(SampleData))
    {
        for(int i =0; i<PER_SIZE;i++)
        {
            buf1[i] = src[i].channel[0];
            buf2[i] = LPF_2Ord_Cal(buf1[i], &lpf_2nd_c_p);	 //二阶低通滤波
        }
        fifo_put_force(fc[0],buf1,PER_SIZE*sizeof(float)); // 通道1 原始数据
        fifo_put_force(fc[1],buf2,PER_SIZE*sizeof(float)); // 通道2 滤波后数据

        memcpy(g_fft_in,buf1,PER_SIZE*sizeof(float));
        fft_execute(real_fft_plan);
        memcpy(out1,real_fft_plan->output,PER_SIZE*sizeof(float));

        memcpy(g_fft_in,buf2,PER_SIZE*sizeof(float));
        fft_execute(real_fft_plan);
        memcpy(out2,real_fft_plan->output,PER_SIZE*sizeof(float));

        memset(buf1,0,PER_SIZE*sizeof(float));
        memset(buf2,0,PER_SIZE*sizeof(float));

        for(int i = 0; i<PER_SIZE; i++)
        {
            if(i>1 && i<PER_SIZE/2)
            {
                buf1[i] = sqrt(pow(out1[2 * i], 2) + pow(out1[2 * i + 1], 2))/ (PER_SIZE/2);
                buf2[i] = sqrt(pow(out2[2 * i], 2) + pow(out2[2 * i + 1], 2))/ (PER_SIZE/2);

            }
            else
            {
                buf1[i] = -10.0;
                buf2[i] = -10.0;
            }
        }

        fifo_put_force(fc[2],buf1,PER_SIZE*sizeof(float)); // 通道3 原始FFT数据
        fifo_put_force(fc[3],buf2,PER_SIZE*sizeof(float)); // 通道4 原始FFT数据
    }


}
*/
void DataProduct::DataHandle(fifo *f)
{

    if(!clean_head(f))
    {
        return;
    }

    SampleData src[PER_SIZE];
    float buf1[PER_SIZE] = {0};
    float buf2[PER_SIZE] = {0};
    float buf3[PER_SIZE] = {0};
    float buf4[PER_SIZE] = {0};
    float out1[PER_SIZE] = {0};
    float out2[PER_SIZE] = {0};
    float out3[PER_SIZE] = {0};
    float out4[PER_SIZE] = {0};

    if(fifo_get(f,src,PER_SIZE*sizeof(SampleData)) == PER_SIZE*sizeof(SampleData))
    {
        for(int i =0; i<PER_SIZE;i++)
        {
            buf1[i] = src[i].channel[0];    //参考
            buf2[i] = src[i].channel[1];    //速度
            buf3[i] =  buf2[i] - buf1[i];   //误差
            buf4[i] = src[i].channel[2];    //输出

        }
        fifo_put_force(fc[0],buf1,PER_SIZE*sizeof(float));
        fifo_put_force(fc[2],buf2,PER_SIZE*sizeof(float));
        fifo_put_force(fc[4],buf3,PER_SIZE*sizeof(float));
        fifo_put_force(fc[6],buf4,PER_SIZE*sizeof(float));

        memcpy(g_fft_in,buf1,PER_SIZE*sizeof(float));
        fft_execute(real_fft_plan);
        memcpy(out1,real_fft_plan->output,PER_SIZE*sizeof(float));

        memcpy(g_fft_in,buf2,PER_SIZE*sizeof(float));
        fft_execute(real_fft_plan);
        memcpy(out2,real_fft_plan->output,PER_SIZE*sizeof(float));

        memcpy(g_fft_in,buf3,PER_SIZE*sizeof(float));
        fft_execute(real_fft_plan);
        memcpy(out3,real_fft_plan->output,PER_SIZE*sizeof(float));

        memcpy(g_fft_in,buf4,PER_SIZE*sizeof(float));
        fft_execute(real_fft_plan);
        memcpy(out4,real_fft_plan->output,PER_SIZE*sizeof(float));

        for(int i = 0; i<PER_SIZE; i++)
        {
            if(i>1 && i<PER_SIZE/2)
            {
                buf1[i] = sqrt(pow(out1[2 * i], 2) + pow(out1[2 * i + 1], 2))/ (PER_SIZE/2);
                buf2[i] = sqrt(pow(out1[2 * i], 2) + pow(out2[2 * i + 1], 2))/ (PER_SIZE/2);
                buf3[i] = sqrt(pow(out1[2 * i], 2) + pow(out3[2 * i + 1], 2))/ (PER_SIZE/2);
                buf4[i] = sqrt(pow(out1[2 * i], 2) + pow(out4[2 * i + 1], 2))/ (PER_SIZE/2);

            }
            else
            {
                buf1[i] = -10.0;
                buf2[i] = -10.0;
                buf3[i] = -10.0;
                buf4[i] = -10.0;
            }
        }

        fifo_put_force(fc[1],buf1,PER_SIZE*sizeof(float));
        fifo_put_force(fc[3],buf2,PER_SIZE*sizeof(float));
        fifo_put_force(fc[5],buf3,PER_SIZE*sizeof(float));
        fifo_put_force(fc[7],buf4,PER_SIZE*sizeof(float));

    }

}

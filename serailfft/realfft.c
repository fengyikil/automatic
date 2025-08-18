#include "fft.h"
#include "realfft.h"
#include "string.h"
#include "stdio.h"
#include "math.h"

#define TWO_PI 6.28318530

float g_fft_in[REAL_FFT_VOLUME]={0};
float g_fft_out[REAL_FFT_VOLUME]={0};
float g_twiddle_factors[2*REAL_FFT_VOLUME]={0};
fft_config_t g_config_fft;

void fpv_fft_init()
{
    g_config_fft.size = REAL_FFT_VOLUME;
    g_config_fft.flags = 0;
    g_config_fft.type = FFT_REAL;
    g_config_fft.direction = FFT_FORWARD;
    g_config_fft.input = g_fft_in;
    g_config_fft.output =g_fft_out;
    g_config_fft.twiddle_factors = g_twiddle_factors;

    float two_pi_by_n = TWO_PI / g_config_fft.size;
    int k,m;
    for (k = 0, m = 0 ; k < g_config_fft.size ; k++, m+=2)
    {
        g_config_fft.twiddle_factors[m] = cosf(two_pi_by_n * k);    // real
        g_config_fft.twiddle_factors[m+1] = sinf(two_pi_by_n * k);  // imag
    }
}

static void fpv_get(FFtPv* fpv,float* buf)
{
    memcpy(buf,fpv->buf,REAL_FFT_VOLUME*sizeof(float));
    fpv->count = 0;
}

/*牺牲一个单元来区分队空和队满，入队时少用一个队列单元。如果数组的大小为Size,那么实际只能存放(Size-1)个元素。*/
static void fpv_put(FFtPv* fpv,float Xn)
{
    if(fpv->count < REAL_FFT_VOLUME)
    {

        fpv->buf[fpv->count] = Xn;
        fpv->count = fpv->count + 1;
    }
}

static int fpv_len(FFtPv* fpv)
{
    return fpv->count;
}


void fpv_sample(FFtPv* fpv,float Xn)
{
    fpv_put(fpv,Xn);
}

void fpv_update(FFtPv* fpv)
{
    float buf[REAL_FFT_VOLUME] = {0};
    fpv_get(fpv,g_fft_in);
    fft_execute(&g_config_fft);
    float max = 0;
    int pos = 0;

    for(int i = 0; i<REAL_FFT_VOLUME; i++)
    {
        printf("g_fft_out is %f\n",g_fft_out[i]);
        if(i>1 && i<REAL_FFT_VOLUME/2)
        {
            buf[i] = sqrt(pow(g_fft_out[2 * i], 2) + pow(g_fft_out[2 * i + 1], 2))/ (REAL_FFT_VOLUME/2);
            if(buf[i]>max)
            {
                max = buf[i];
                pos = i;
            }
        }
    }
    fpv->pos = pos;
    fpv->value = max;
}

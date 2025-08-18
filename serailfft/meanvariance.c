#include "meanvariance.h"
#include "math.h"
#include "stdio.h"

static void mean_variance_get(MeanVariance* mt,float* Xp)
{
    if(mt->front == mt->rear )
    {
        return;
    }
    *Xp = mt->buf[mt->front];
    mt->front = (mt->front + 1)%MEAN_VARIANCE_LEN;
}

/*牺牲一个单元来区分队空和队满，入队时少用一个队列单元。如果数组的大小为Size,那么实际只能存放(Size-1)个元素。*/
static void mean_variance_put(MeanVariance* mt,float Xn)
{
    if((mt->rear+1)%MEAN_VARIANCE_LEN==mt->front) {
        return;
    }
    mt->buf[mt->rear]=Xn; //插入数据元素
    mt->rear=(mt->rear+1)%MEAN_VARIANCE_LEN; //队尾指针后移一位
}

static int mean_variance_len(MeanVariance* mt)
{
    return (mt->rear-mt->front+MEAN_VARIANCE_LEN)%MEAN_VARIANCE_LEN;
}

void mean_variance_update(MeanVariance* mt,float Xn)
{
    float n,N;
    float m,m1,v,Xp;
    n = mean_variance_len(mt);
    N =  MEAN_VARIANCE_VOLUME;

    m = mt->mean;
    v = mt->variance;

    if(n<N)
    {
        if(n == 0)
        {
            mt->mean = Xn;
            mt->variance = 0;
        }
        else
        {   n=n+1;
            mt->mean = (1.0/n)*((n-1.0)*m + Xn);
            m = mt->mean;
            mt->variance = (1.0/(n-1.0))*((n-2.0)*v + ((n*1.0)/(n-1.0))*(m-Xn)*(m-Xn));

        }
    }
    else
    {
        mean_variance_get(mt,&Xp);
        //printf("n-N:%f %f %f\n",n,N,Xp);

        mt->mean = m + (1.0/n)*(Xn - Xp);
        m1 = mt->mean;
        mt->variance = v + ((m1 - m)/(n-1))*((Xn+Xp)*(n-1)-2*n*m+2*Xp);
    }
    mean_variance_put(mt,Xn);
}

void mean_variance_reset(MeanVariance *mt)
{
    mt->front = 0;
    mt->rear = 0;
    mt->mean = 0;
    mt->variance = 0;
}

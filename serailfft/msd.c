#include "msd.h"
#include "fifo.h"
#include "math.h"
#include "stdio.h"

MsdType* msd_init(unsigned int size)
{
    MsdType* mt = (MsdType *)malloc(sizeof(MsdType));
    if (!mt)
    {
        printf("malloc erro!\n");
        return NULL;
    }
    mt->data = fifo_init(size);
    if(mt->data == NULL)
    {
        fifo_free(mt);
        return NULL;
    }

    mt->mean = 0;
    mt->variance = 0;
    return mt;
}

MsdType* msd_update(MsdType* mt,float Xn)
{
    float n,N;
    float m,m1,v,Xp;
    n = fifo_len(mt->data)/sizeof(float);
    N = (mt->data->size)/sizeof(float);

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
//            mt->variance = ((n-1.0f)/(n*n))*(Xn-m)*(Xn-m) + ((n-1.0f)/n)*v;

        }
    }
    else
    {
        fifo_get(mt->data,&Xp,sizeof(float));
        //printf("n-N:%f %f %f\n",n,N,Xp);

        mt->mean = m + (1.0/n)*(Xn - Xp);
        m1 = mt->mean;
        mt->variance = v + ((m1 - m)/(n-1))*((Xn+Xp)*(n-1)-2*n*m+2*Xp);
    }
    fifo_put(mt->data,&Xn,sizeof(float));
}

#include "fifo.h"
#include <malloc.h>
#include <stdio.h>
#include<string.h>
//判断x是否是2的次方
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x)-1)) == 0))
//取a和b中最小值
#define min(a, b) (((a) < (b)) ? (a) : (b))
static inline unsigned int fls(unsigned int x)
{
    unsigned int position;
    unsigned int i;
    if (0 != x)
    {
        for (i = (x >> 1), position = 0; i != 0; ++position)
            i >>= 1;
    }
    else
    {
        position = -1;
    }
    return position + 1;
}
static inline unsigned int roundup_pow_of_two(unsigned int x)
{
    return 1UL << fls(x - 1);
}
struct fifo *fifo_init(unsigned int size)
{
    struct fifo *f = NULL;
    if (!is_power_of_2(size))
    {
        size = roundup_pow_of_two(size);
    }
    f = (struct fifo *)malloc(sizeof(struct fifo));
    if (!f)
    {
        printf("malloc erro!\n");
        goto Lerr;
    }
    memset(f, 0, sizeof(struct fifo));
    f->buffer = (unsigned char *)malloc(size);
    if (!f->buffer)
    {
        printf("malloc erro!\n");
        goto Lerr;
    }

    f->size = size;
    f->in = 0;
    f->out = 0;
    return f;
Lerr:
    if (f)
    {
        if (f->buffer)
            free(f->buffer);
        free(f);
    }
    return NULL;
}

void fifo_free(struct fifo *f)
{
    if (f)
    {
        if (f->buffer)
        {
            free(f->buffer);
            f->buffer = NULL;
        }
        free(f);
        f = NULL;
    }
}
unsigned int fifo_len(struct fifo *f)
{
    return (f->in - f->out);
}
void fifo_reset(struct fifo *f)
{
    f->in = f->out = 0;
}
unsigned int fifo_get(struct fifo *f, void *buf, unsigned int len)
{
    unsigned int l;
    unsigned char *buffer = buf;
    l = f->in - f->out;
    if (len > l)
    {
//        printf("fifo not enough len is %d\t fifo.len is %d!\n", len, (f->in - f->out));
        return (l);
    }
    else
    {
        l = min(len, f->size - (f->out & (f->size - 1)));
        memcpy(buffer, f->buffer + (f->out & (f->size - 1)), l);
        memcpy(buffer + l, f->buffer, len - l);
        f->out += len;
        return len;
    }
}

unsigned int fifo_get_no_out(struct fifo *f, void *buf, unsigned int len)
{
    unsigned short l;
    unsigned char *buffer = buf;
    l = f->in - f->out;
    if (len > l) {  //判断数据是否够长
        return (l);
    } else {
        l = min(len, f->size - (f->out & (f->size - 1)));
        memcpy(buffer, f->buffer + (f->out & (f->size - 1)), l);
        memcpy(buffer + l, f->buffer, len - l);
        return len;
    }
}

unsigned int fifo_out(struct fifo* f, unsigned int len)
{
    unsigned short l;
    l = f->in - f->out;
    if (len > l) {  //判断数据是否够长
        return 0;
    } else {
        f->out += len;
        return len;
    }
}

unsigned int fifo_put(struct fifo *f, void *buf, unsigned int len)
{
    unsigned int l;
    unsigned char *buffer = buf;
    l = f->size - f->in + f->out;
    if (len > l)
    {
        printf("fifo have no such space!\t len is %d \t fifo.space is %d\n", len, l);
        return (l);
    }
    else
    {
        l = min(len, f->size - (f->in & (f->size - 1)));
        memcpy(f->buffer + (f->in & (f->size - 1)), buffer, l);
        memcpy(f->buffer, buffer + l, len - l);
        f->in += len;
        return len;
    }
}

unsigned int fifo_put_force(struct fifo *f, void *buf, unsigned int len)
{
    unsigned int l,m_len;
    unsigned char *buffer = buf;

    l = f->size - f->in + f->out;

    if(len > f->size)
    {
        f->in = f->out = 0;
        m_len = f->size;
    }
    else if(len > l)
    {
        f->out = f->out + (len - l);
        m_len = len;
    }
    else
    {
        m_len = len;
    }


    l = min(m_len, f->size - (f->in & (f->size - 1)));
    memcpy(f->buffer + (f->in & (f->size - 1)), buffer, l);
    memcpy(f->buffer, buffer + l, m_len - l);
    f->in += m_len;

    return m_len;
}

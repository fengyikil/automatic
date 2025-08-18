#ifndef __FIFO_H__
#define __FIFO_H__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
struct fifo
{
    unsigned char *buffer;
    unsigned int size;
    unsigned int in;
    unsigned int out;
};

struct fifo *fifo_init(unsigned int size);
void fifo_free(struct fifo *f);
unsigned int fifo_len(struct fifo *f);
void fifo_reset(struct fifo *f);
unsigned int fifo_get(struct fifo *f, void *buffer, unsigned int len);
unsigned int fifo_get_no_out(struct fifo *f, void *buf, unsigned int len);
unsigned int fifo_out(struct fifo* f, unsigned int len);
unsigned int fifo_put(struct fifo *f, void *buffer, unsigned int len);
unsigned int fifo_put_force(struct fifo *f, void *buffer, unsigned int len);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif

#ifndef ringbuffer_h
#define ringbuffer_h

#include <bstrlib/bstrlib.h>
#include <sys/types.h>

#define RINGBUFFER_OK        (0)
#define RINGBUFFER_ERR_NULL  (-1)
#define RINGBUFFER_ERR_EMPTY (-2)
#define RINGBUFFER_ERR_FULL  (-3)

typedef struct {
    char *start_ptr;
    char *end_ptr;
    char *read_ptr;
    char *write_ptr;
    size_t length;
    size_t count;
} RingBuffer;

RingBuffer *RingBuffer_create(size_t length);

void RingBuffer_destroy(RingBuffer *buffer);

int RingBuffer_read(RingBuffer *buf, char *target, size_t amount);

int RingBuffer_write(RingBuffer *buf, char *data, size_t length);

ssize_t RingBuffer_writefd(RingBuffer *buf, int fd);
ssize_t RingBuffer_send(RingBuffer *buf, int socket, int flags);

ssize_t RingBuffer_readfd(RingBuffer *buf, int fd);
ssize_t RingBuffer_recv(RingBuffer *buf, int socket, int flags);

char RingBuffer_empty(RingBuffer *buf);

char RingBuffer_full(RingBuffer *buf);

size_t RingBuffer_available_data(RingBuffer *buf);

size_t RingBuffer_available_space(RingBuffer *buf);

int RingBuffer_gets(RingBuffer *buf, bstring s);

#define RingBuffer_available_data(B) ((B)->count)

#define RingBuffer_available_space(B) ((B)->length - (B)->count)

#define RingBuffer_available_cspace(B)                        \
    ((size_t)((B)->count == 0 ? (B)->end_ptr - (B)->write_ptr \
              : (B)->write_ptr > (B)->read_ptr                \
                  ? (B)->end_ptr - (B)->write_ptr             \
                  : (B)->read_ptr - (B)->write_ptr))

#define RingBuffer_available_cdata(B)              \
    ((size_t)((B)->count == 0 ? 0                  \
              : (B)->write_ptr > (B)->read_ptr     \
                  ? (B)->write_ptr - (B)->read_ptr \
                  : (B)->end_ptr - (B)->read_ptr))

#define RingBuffer_full(B) ((B)->length == (B)->count)

#define RingBuffer_empty(B) ((B)->count == 0)

#define RingBuffer_puts(B, S) RingBuffer_write((B), bdata((S)), blength((S)))

#define RingBuffer_get_all(B, S) \
    RingBuffer_gets((B), RingBuffer_available_data(B))

#define RingBuffer_start_ptr(B) ((B)->start_ptr)

#define RingBuffer_end_ptr(B) ((B)->end_ptr)

#endif

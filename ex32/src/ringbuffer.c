#include "dbg.h"
#include <ringbuffer.h>
#include <stdlib.h>

RingBuffer *RingBuffer_create(size_t length) {
    RingBuffer *ret = NULL;
    CHECK(length > 2, "wrong length.");
    ret = calloc(1, sizeof(RingBuffer));
    CHECK_MEM(ret);
    ret->start = calloc(length, sizeof(char));
    CHECK_MEM(ret->start);
    ret->end = ret->start + length;
    ret->read_ptr = ret->start;
    ret->write_ptr = ret->start;
    ret->length = length;
    return ret;
error:
    if (ret != NULL) {
        if (ret->start != NULL) {
            free(ret->start);
        }
        free(ret);
    }
    return NULL;
}

void RingBuffer_destroy(RingBuffer *buffer) {
    if (buffer != NULL) {
        if (buffer->start != NULL) {
            free(buffer->start);
        }
        free(buffer);
    }
}

int RingBuffer_write(RingBuffer *buf, char *data, size_t amount) {
    int rc = 0;
    CHECKRC(buf != NULL, RINGBUFFER_ERR_NULL, "Null buffer.");
    CHECKRC(RingBuffer_available_space(buf) >= amount, RINGBUFFER_ERR_FULL,
            "Buffer is full (not enough space).");

    for (size_t i = 0; i < amount; i++) {
        LOG_DEBUG("space %lu data %lu w %d r %d l %d",
                  RingBuffer_available_space(buf),
                  RingBuffer_available_data(buf), buf->write_ptr, buf->read_ptr,
                  buf->length);
        *(buf->write_ptr) = *(data + i);
        buf->count++;
        LOG_DEBUG("writing %c at %d", *(data + i),
                  (buf->write_ptr) - buf->start);

        buf->write_ptr =
            buf->write_ptr >= buf->end - 1 ? buf->start : buf->write_ptr + 1;
    }
error:
    return rc;
}

int RingBuffer_readchar(RingBuffer *buf, char *target) {
    int rc = 0;
    CHECKRC(buf != NULL, RINGBUFFER_ERR_NULL, "Null buffer.");
    CHECKRC(RingBuffer_available_data(buf) > 0, RINGBUFFER_ERR_EMPTY,
            "Buffer is empty.");

    *(target) = *(buf->read_ptr);
    buf->read_ptr =
        buf->read_ptr >= buf->end - 1 ? buf->start : buf->read_ptr + 1;
    buf->count--;

error:
    return rc;
}

int RingBuffer_gets(RingBuffer *buf, bstring s) {
    char ch = 0;
    int rc = 0;
    size_t avail = RingBuffer_available_data(buf);
    for (size_t i = 0; i < avail; i++) {
        rc = RingBuffer_readchar(buf, &ch);
        LOG_DEBUG("%c", ch);
        CHECK(rc == 0, "Error reading from ring buffer");
        bconchar(s, ch);
    }
error:
    return rc;
}

int RingBuffer_read(RingBuffer *buf, char *target, size_t amount) {
    int rc = 0;
    CHECKRC(buf != NULL, RINGBUFFER_ERR_NULL, "Null buffer.");
    CHECKRC(RingBuffer_available_data(buf) < amount, RINGBUFFER_ERR_EMPTY,
            "Buffer is empty (not enough data).");

    for (size_t i = 0; i < amount; i++) {
        *(target + i) = *(buf->read_ptr);

        buf->read_ptr =
            buf->read_ptr >= buf->end - 1 ? buf->start : buf->read_ptr + 1;
        buf->count--;
    }
error:
    return rc;
}

#include "dbg.h"
#include <ringbuffer.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

RingBuffer *RingBuffer_create(size_t length) {
    RingBuffer *ret = NULL;
    CHECK(length > 2, "wrong length.");
    ret = calloc(1, sizeof(RingBuffer));
    CHECK_MEM(ret);
    ret->start_ptr = calloc(length, sizeof(char));
    CHECK_MEM(ret->start_ptr);
    ret->end_ptr = ret->start_ptr + length;
    ret->read_ptr = ret->start_ptr;
    ret->write_ptr = ret->start_ptr;
    ret->length = length;
    return ret;
error:
    if (ret != NULL) {
        if (ret->start_ptr != NULL) {
            free(ret->start_ptr);
        }
        free(ret);
    }
    return NULL;
}

void RingBuffer_destroy(RingBuffer *buffer) {
    if (buffer != NULL) {
        if (buffer->start_ptr != NULL) {
            free(buffer->start_ptr);
        }
        free(buffer);
    }
}

ssize_t RingBuffer_writefd(RingBuffer *buf, int fd) {
    ssize_t rc = 0;
    size_t to_write = 0;
    CHECKRC(buf != NULL, RINGBUFFER_ERR_NULL, "Null buffer.");
    CHECKRC(RingBuffer_available_data(buf) > 0, RINGBUFFER_ERR_EMPTY,
            "Buffer is empty (not enough data).");

    to_write = RingBuffer_available_cdata(buf);

    rc = write(fd, buf->read_ptr, to_write);
    CHECKRC(rc >= 0, rc, "Error writing to file.");
    buf->count -= rc;
    buf->read_ptr += rc;
    if (buf->read_ptr >= buf->end_ptr) {
        buf->read_ptr = buf->start_ptr;
    }
error:
    return rc;
}
ssize_t RingBuffer_readfd(RingBuffer *buf, int fd) {
    ssize_t rc = 0;
    size_t to_read = 0;
    CHECKRC(buf != NULL, RINGBUFFER_ERR_NULL, "Null buffer.");
    LOG_DEBUG("read some from fd %d, ad %lu acd %zu", fd, RingBuffer_available_space(buf), RingBuffer_available_cspace(buf));
    CHECKRC(RingBuffer_available_space(buf) > 0, RINGBUFFER_ERR_FULL,
            "Buffer is full (not enough space).");

    to_read = RingBuffer_available_cspace(buf);

    rc = read(fd, buf->write_ptr, to_read);
    CHECKRC(rc >= 0, rc, "Error reading from file.");
    buf->count += rc;
    buf->write_ptr += rc;
    if (buf->write_ptr >= buf->end_ptr) {
        buf->write_ptr = buf->start_ptr;
    }
error:
    return rc;
}

ssize_t RingBuffer_recv(RingBuffer *buf, int socket, int flags) {
    ssize_t rc = 0;
    size_t to_read = 0;
    CHECKRC(buf != NULL, RINGBUFFER_ERR_NULL, "Null buffer.");
    CHECKRC(RingBuffer_available_space(buf) > 0, RINGBUFFER_ERR_FULL,
            "Buffer is full (not enough space).");

    to_read = RingBuffer_available_cspace(buf);

    rc = recv(socket, buf->write_ptr, to_read, flags);
    CHECKRC(rc >= 0, rc, "Error reading from socket.");
    buf->count += rc;
    buf->write_ptr += rc;
    if (buf->write_ptr >= buf->end_ptr) {
        buf->write_ptr = buf->start_ptr;
    }
error:
    return rc;
}

ssize_t RingBuffer_send(RingBuffer *buf, int socket, int flags) {
    ssize_t rc = 0;
    size_t to_write = 0;
    CHECKRC(buf != NULL, RINGBUFFER_ERR_NULL, "Null buffer.");
    CHECKRC(RingBuffer_available_data(buf) > 0, RINGBUFFER_ERR_EMPTY,
            "Buffer is empty (not enough data).");

    to_write = RingBuffer_available_cdata(buf);

    LOG_DEBUG("send some to fd %d, ad %lu acd %zu", socket, RingBuffer_available_data(buf), RingBuffer_available_cdata(buf));

    rc = send(socket, buf->read_ptr, to_write, flags);
    CHECKRC(rc >= 0, rc, "Error writing to socket.");
    buf->count -= rc;
    buf->read_ptr += rc;
    if (buf->read_ptr >= buf->end_ptr) {
        buf->read_ptr = buf->start_ptr;
    }
error:
    return rc;
}

int RingBuffer_write(RingBuffer *buf, char *data, size_t amount) {
    int rc = 0;
    CHECKRC(buf != NULL, RINGBUFFER_ERR_NULL, "Null buffer.");
    CHECKRC(RingBuffer_available_space(buf) >= amount, RINGBUFFER_ERR_FULL,
            "Buffer is full (not enough space).");

    if (amount <= RingBuffer_available_cspace(buf)) {
        memcpy(buf->write_ptr, data, amount);
        buf->count += amount;
        buf->write_ptr += amount;
        if (buf->write_ptr >= buf->end_ptr) {
            buf->write_ptr = buf->start_ptr;
        }
    } else {
        // write with wrap
        size_t wrote = RingBuffer_available_cspace(buf);
        memcpy(buf->write_ptr, data, wrote);
        buf->write_ptr = buf->start_ptr;
        memcpy(buf->write_ptr, data + wrote, amount - wrote);
        buf->write_ptr += (amount - wrote);
        buf->count += amount;
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
        buf->read_ptr >= buf->end_ptr - 1 ? buf->start_ptr : buf->read_ptr + 1;
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

    if (amount <= RingBuffer_available_cdata(buf)) {
        memcpy(target, buf->read_ptr, amount);
        buf->count -= amount;
        buf->read_ptr += amount;
        if (buf->read_ptr >= buf->end_ptr) {
            buf->read_ptr = buf->start_ptr;
        }
    } else {
        // read with wrap
        size_t read = RingBuffer_available_cdata(buf);
        memcpy(target, buf->read_ptr, read);
        buf->read_ptr = buf->start_ptr;
        memcpy(target, buf->read_ptr, amount - read);
        buf->read_ptr += (amount - read);
        buf->count -= amount;
    }
error:
    return rc;
}

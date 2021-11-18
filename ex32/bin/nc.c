#include <dbg.h>
#include <ringbuffer.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>

struct tagbstring NL = bsStatic("\n");
struct tagbstring CRLF = bsStatic("\r\n");

#define FD 0

int nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    int rc = 0;
    CHECK(flags >= 0, "Invalid flags on nonblock.");

    rc = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    CHECK(rc == 0, "Can't set nonblocking.");

    return 0;

error:
    return -1;
}

int client_connect(char *host, char *port) {
    int rc = 0;
    struct addrinfo *addr = NULL;

    rc = getaddrinfo(host, port, NULL, &addr);
    CHECK(rc == 0, "Failed to lookup %s:%s", host, port);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    CHECK(sock >= 0, "Cannot create a socket");

    rc = connect(sock, addr->ai_addr, addr->ai_addrlen);
    CHECK(rc == 0, "Cannot create a socket.");
   // rc = nonblock(sock);
   // CHECK(rc == 0, "Cannot set nonblocking.");
    freeaddrinfo(addr);
    return sock;
error:
    freeaddrinfo(addr);
    return -1;
}

int read_some(RingBuffer *buf, int fd, int is_socket) {
    int rc = 0;

    if (!RingBuffer_full(buf)) {
        if (is_socket) {
            rc = RingBuffer_recv(buf, fd, 0);
        } else {
            rc = RingBuffer_readfd(buf, fd);
        }
    }

    CHECK(rc >= 0, "Failed to read from fd: %d", fd);

// fallthrough
error:
    return rc;
}

int write_some(RingBuffer *buf, int fd, int is_socket) {
    int rc = 0;

    if (RingBuffer_empty(buf)) {
        return 0;
    }

    bstring data = bfromcstr("");
    rc = RingBuffer_gets(buf, data);

    CHECK(rc == 0, "Error reading from RingBuffer.");

    if (blength(data) > 0) {
        CHECK(bfindreplace(data, &NL, &CRLF, 0) == 0, "Error replacing NL");
        if (is_socket) {
            rc = send(fd, bdata(data), blength(data), 0);
        } else {
            rc = write(fd, bdata(data), blength(data));
        }
        CHECK(rc == blength(data), "Failed to write everything to fd: %d.", fd);
    }

// fallthrough
error:
    bdestroy(data);
    return rc;
}
int main(int argc, char *argv[]) {
    int rc = 0;
    ssize_t ret = 0;
    char exit = 0;

    RingBuffer *in_buf = NULL;
    RingBuffer *out_buf = NULL;

    fd_set fd_read;
    fd_set fd_read_mod;
    char buf[10];
    struct timeval timeout = {0};

    int socket = 0;
    if (argc != 3) {
        printf("Usage: %s host port\n", argv[0]);
        goto exit;
    }

    socket = client_connect(argv[1], argv[2]);
    CHECK(socket >= 0, "Connection to %s:%s failed", argv[1], argv[2]);
    FD_ZERO(&fd_read);
    FD_SET(0, &fd_read);
    FD_SET(socket, &fd_read);
    in_buf = RingBuffer_create(10);
    out_buf = RingBuffer_create(10);

    CHECK(in_buf != NULL, "Failed to create in_buf");
    CHECK(out_buf != NULL, "Failed to create out_buf");

    while (!exit) {
        fd_read_mod = fd_read;
        timeout.tv_sec = 50;
        ret = select(socket + 1, &fd_read_mod, NULL, NULL, &timeout);
        LOG_DEBUG("select returned %zd", ret);
        CHECK(ret >= 0, "select() failed.");
        if (ret == 0) {
            LOG_INFO("timeout.");
            break;
        }
        if (FD_ISSET(0, &fd_read_mod)) {
            rc = read_some(out_buf, 0, 0);
            LOG_DEBUG("read from 0, rc %d", rc);
            CHECK(rc >= 0, "Failed to read from stdin.");
        }
        if (FD_ISSET(socket, &fd_read_mod)) {
            rc = read_some(in_buf, socket, 1);
            LOG_DEBUG("read from socket, rc %d, buf count %zu buf len %zu", rc,
                      in_buf->count, in_buf->length);
            CHECK(rc >= 0, "Failed to read from socket.");
            if (rc == 0) {
                exit = 1;
            }
        }

        while (!RingBuffer_empty(in_buf)) {
            rc = write_some(in_buf, 1, 0);
            CHECK(rc >= 0, "Failed to write to stdout.");
            LOG_DEBUG("write to stdout rc %d", rc);
        }

        while (!RingBuffer_empty(out_buf)) {
            rc = write_some(out_buf, socket, 1);
            CHECK(rc >= 0, "Failed to write to socket.");
            LOG_DEBUG("write to socket, rc %d", rc);
        }
    }

exit:
    if (in_buf != NULL)
        RingBuffer_destroy(in_buf);
    if (out_buf != NULL)
        RingBuffer_destroy(out_buf);
    if (socket > 0) {
        shutdown(socket, SHUT_RDWR);
        close(socket);
    }
    return rc;
error:
    rc = -1;
    goto exit;
}

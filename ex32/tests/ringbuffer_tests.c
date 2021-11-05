#include "minunit.h"
#include <assert.h>
#include <ringbuffer.h>

char *test_simple() {
    RingBuffer *rb = NULL;
    struct tagbstring test1 = bsStatic("test");
    rb = RingBuffer_create(5);
    MU_ASSERT(rb != NULL, "Failed to create ring buffer.");
    MU_ASSERT(rb != NULL, "Failed to make ring buffer #2");

    MU_ASSERT(RingBuffer_puts(rb, &test1) == 0, "Error writing string");
    MU_ASSERT(RingBuffer_available_space(rb) == 1, "Got %lu", RingBuffer_available_space(rb));
    MU_ASSERT(RingBuffer_available_data(rb) == 4, "got %lu %p %p", RingBuffer_available_data(rb), rb->write_ptr, rb->read_ptr);

    bstring res = bfromcstr("");
    RingBuffer_gets(rb, res);
    MU_ASSERT(blength(res) == 4, "Got %lu", RingBuffer_available_space(rb));

    MU_ASSERT(RingBuffer_available_space(rb) == 5, "Got %lu", RingBuffer_available_space(rb));
    MU_ASSERT(RingBuffer_available_data(rb) == 0, "");

    MU_ASSERT(RingBuffer_puts(rb, &test1) == 0, "Error writing string");
    MU_ASSERT(RingBuffer_available_space(rb) == 1, "got %lu %p %p", RingBuffer_available_space(rb), rb->write_ptr, rb->read_ptr);
    MU_ASSERT(RingBuffer_available_data(rb) == 4, "");

    RingBuffer_gets(rb, res);
    LOG_DEBUG("%s", bdata(res));
    MU_ASSERT(blength(res) == 8, "Got %lu", RingBuffer_available_space(rb));
    MU_ASSERT(strcmp(bdata(res), "testtest") == 0, "Got %s", bdata(res));

    MU_ASSERT(RingBuffer_available_space(rb) == 5, "Got %lu", RingBuffer_available_space(rb));
    MU_ASSERT(RingBuffer_available_data(rb) == 0, "");

    bdestroy(res);
    RingBuffer_destroy(rb);

    return NULL;

}

char *test_small() {
    RingBuffer *rb = NULL;
    rb = RingBuffer_create(0);
    MU_ASSERT(rb == NULL, "qq");
    rb = RingBuffer_create(3);
    MU_ASSERT(rb != NULL, "");

    struct tagbstring test1 = bsStatic("test");
    MU_ASSERT(RingBuffer_puts(rb, &test1) == RINGBUFFER_ERR_FULL, "");
    struct tagbstring test2 = bsStatic("t");
    MU_ASSERT(RingBuffer_puts(rb, &test2) == RINGBUFFER_OK, "Error writing string");
    MU_ASSERT(RingBuffer_available_space(rb) == 2, "Got %lu", RingBuffer_available_space(rb));
    MU_ASSERT(RingBuffer_available_data(rb) == 1, "got %lu %p %p", RingBuffer_available_data(rb), rb->write_ptr, rb->read_ptr);
    bstring res = bfromcstr("");
    RingBuffer_gets(rb, res);
    MU_ASSERT(blength(res) == 1, "Got %lu", RingBuffer_available_space(rb));
    MU_ASSERT(RingBuffer_available_space(rb) == 3, "Got %lu", RingBuffer_available_space(rb));
    MU_ASSERT(RingBuffer_available_data(rb) == 0, "");

    struct tagbstring test3 = bsStatic("xc");
    MU_ASSERT(RingBuffer_puts(rb, &test3) == RINGBUFFER_OK, "Error writing string");
    MU_ASSERT(RingBuffer_available_space(rb) == 1, "got %lu %p %p", RingBuffer_available_data(rb), rb->write_ptr, rb->read_ptr);
    MU_ASSERT(RingBuffer_available_data(rb) == 2, "got %lu %p %p", RingBuffer_available_data(rb), rb->write_ptr, rb->read_ptr);
    struct tagbstring test4 = bsStatic("z");
    MU_ASSERT(RingBuffer_puts(rb, &test4) == RINGBUFFER_OK, "Error writing string");
    MU_ASSERT(RingBuffer_available_space(rb) == 0, "got %lu %p %p", RingBuffer_available_space(rb), rb->write_ptr, rb->read_ptr);
    MU_ASSERT(RingBuffer_available_data(rb) == 3, "got %lu %p %p", RingBuffer_available_data(rb), rb->write_ptr, rb->read_ptr);

    RingBuffer_gets(rb, res);
    MU_ASSERT(blength(res) == 4, "Got %lu", RingBuffer_available_space(rb));
    MU_ASSERT(RingBuffer_available_space(rb) == 3, "Got %lu", RingBuffer_available_space(rb));
    MU_ASSERT(RingBuffer_available_data(rb) == 0, "");

    MU_ASSERT(strcmp(bdata(res), "txcz") == 0, "Got %s", bdata(res));

    bdestroy(res);
    RingBuffer_destroy(rb);

    return NULL;
}

char *all_tests() {
    MU_SUITE_START();

    MU_RUN_TEST(test_simple);
    MU_RUN_TEST(test_small);

    return NULL;
}

RUN_TESTS(all_tests);

#include "minunit.h"
#include <assert.h>
#include <queue.h>

char *tests[] = {"test1 data", "test2 data", "test3 data"};

#define NUM_TESTS 3

char *test_simple() {
    Queue *queue = NULL;
    queue = Queue_create();
    MU_ASSERT(queue != NULL, "Failed to create queue.");
    MU_ASSERT(queue != NULL, "Failed to make queue #2");

    int i = 0;
    for (i = 0; i < NUM_TESTS; i++) {
        Queue_send(queue, tests[i]);
        MU_ASSERT(Queue_peek(queue) == tests[i], "Wrong next value.");
    }

    MU_ASSERT(Queue_count(queue) == NUM_TESTS, "Wrong count on push.");

    QUEUE_FOREACH(queue, cur) { LOG_DEBUG("VAL: %s", (char *)cur); }

    for (i = NUM_TESTS - 1; i >= 0; i--) {
        char *val = Queue_recv(queue);
        MU_ASSERT(val == tests[i], "Wrong value on pop.");
    }

    MU_ASSERT(Queue_count(queue) == 0, "Wrong count after pop.");

    Queue_destroy(queue);

    return NULL;
}

char *all_tests() {
    MU_SUITE_START();

    MU_RUN_TEST(test_simple);

    return NULL;
}

RUN_TESTS(all_tests);

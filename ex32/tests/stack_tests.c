#include "minunit.h"
#include <assert.h>
#include <stack.h>

char *tests[] = {"test1 data", "test2 data", "test3 data"};

#define NUM_TESTS 3

char *test_simple() {
    Stack *stack = NULL;
    stack = Stack_create();
    MU_ASSERT(stack != NULL, "Failed to create stack.");
    MU_ASSERT(stack != NULL, "Failed to make stack #2");

    int i = 0;
    for (i = 0; i < NUM_TESTS; i++) {
        Stack_push(stack, tests[i]);
        MU_ASSERT(Stack_peek(stack) == tests[i], "Wrong next value.");
    }

    MU_ASSERT(Stack_count(stack) == NUM_TESTS, "Wrong count on push.");

    STACK_FOREACH(stack, cur) { LOG_DEBUG("VAL: %s", (char *)cur); }

    for (i = NUM_TESTS - 1; i >= 0; i--) {
        char *val = Stack_pop(stack);
        MU_ASSERT(val == tests[i], "Wrong value on pop.");
    }

    MU_ASSERT(Stack_count(stack) == 0, "Wrong count after pop.");

    Stack_destroy(stack);

    return NULL;
}

char *all_tests() {
    MU_SUITE_START();

    MU_RUN_TEST(test_simple);

    return NULL;
}

RUN_TESTS(all_tests);

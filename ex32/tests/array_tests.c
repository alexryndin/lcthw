#include "../src/array.h"
#include "minunit.h"

static Array *array = NULL;
static int *val1 = NULL;
static int *val2 = NULL;

char *test_create() {
    array = Array_create(sizeof(int), 100);
    MU_ASSERT(array != NULL, "Array_create failed.");
    MU_ASSERT(array->contents != NULL, "contents are wrong in array");
    MU_ASSERT(array->end == 0, "end isn't at the right spot");
    MU_ASSERT(array->element_size == sizeof(int), "element size is wrong.");
    MU_ASSERT(array->max == 100, "wrong max length on initial size");

    return NULL;
}

char *test_destroy() {
    Array_destroy(array);

    return NULL;
}

char *test_new() {
    val1 = Array_new(array);
    MU_ASSERT(val1 != NULL, "failed to make a new element");

    val2 = Array_new(array);
    MU_ASSERT(val2 != NULL, "failed to make a new element");

    return NULL;
}

char *test_set() {
    Array_set(array, 0, val1);
    Array_set(array, 1, val2);

    return NULL;
}

char *test_get() {
    MU_ASSERT(Array_get(array, 0) == val1, "Wrong first value.");
    MU_ASSERT(Array_get(array, 1) == val2, "Wrong second value.");

    return NULL;
}

char *test_remove() {
    int *val_check = Array_remove(array, 0);

    MU_ASSERT(val_check != NULL, "Should not get NULL.");
    MU_ASSERT(*val_check == *val1, "Should get the first value.");
    MU_ASSERT(Array_get(array, 0) == NULL, "Should be gone.");
    Array_free(val_check);

    val_check = Array_remove(array, 1);
    MU_ASSERT(val_check != NULL, "Should not get NULL.");
    MU_ASSERT(*val_check == *val2, "Should get the second value.");
    MU_ASSERT(Array_get(array, 1) == NULL, "Should be gone.");
    Array_free(val_check);

    return NULL;
}

char *test_expand() {
    int old_max = array->max;
    MU_ASSERT(Array_expand(array) == 0, "Error on array expanding");
    MU_ASSERT(array->max == (size_t)(old_max * array->expand_factor + 1),
              "Wrong size after expand.");

    // Array_contract(array);
    // MU_ASSERT((unsigned int)array->max == array->expand_factor + 1,
    //           "Should stay at the expand_rate at least.");
    // Array_contract(array);
    // MU_ASSERT((unsigned int)array->max == array->expand_factor + 1,
    //           "Should stay at the expand_rate at least.")
    //
    return NULL;
}

char *test_push_pop() {
    int i = 0;
    for (i = 0; i < 1000; i++) {
        int *val = Array_new(array);
        *val = i * 333;
        Array_push(array, val);
    }

    MU_ASSERT(array->max == 1154, "Wrong max size.");

    for (i = 999; i >= 0; i--) {
        int *val = Array_pop(array);
        LOG_DEBUG("%d %d %d", *val, i*333, i);
        MU_ASSERT(val != NULL, "Should't get a NULL");
        MU_ASSERT(*val == i * 333, "Wrong value.");
        Array_free(val);
    }

    return NULL;
}

char *all_tests() {
    MU_SUITE_START();
    MU_RUN_TEST(test_create);
    MU_RUN_TEST(test_new);
    MU_RUN_TEST(test_set);
    MU_RUN_TEST(test_get);
    MU_RUN_TEST(test_remove);
    MU_RUN_TEST(test_expand);
    MU_RUN_TEST(test_push_pop);
    MU_RUN_TEST(test_destroy);

    return NULL;
}

RUN_TESTS(all_tests);

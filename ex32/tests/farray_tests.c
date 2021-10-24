#include "../src/farray.h"
#include "minunit.h"

static FArray *array = NULL;
static int *val1 = NULL;
static int *val2 = NULL;

char *test_create() {
    array = FArray_create(sizeof(void *), sizeof(int), 100);
    MU_ASSERT(array != NULL, "FArray_create failed.");
    MU_ASSERT(array->contents != NULL, "contents are wrong in array");
    MU_ASSERT(array->end == 0, "end isn't at the right spot");
    MU_ASSERT(array->element_size == sizeof(void *), "element size is wrong.");
    MU_ASSERT(array->max == 100, "wrong max length on initial size");

    return NULL;
}

char *test_destroy() {
    FArray_destroy(array);

    return NULL;
}

char *test_new() {
    val1 = FArray_new(array);
    MU_ASSERT(val1 != NULL, "failed to make a new element");

    val2 = FArray_new(array);
    MU_ASSERT(val2 != NULL, "failed to make a new element");

    return NULL;
}

char *test_set() {
    FArray_set_pointer(array, 0, val1);
    FArray_set_pointer(array, 1, val2);

    return NULL;
}

char *test_get() {
    MU_ASSERT(FArray_get_pointer(array, 0) == val1, "Wrong first value.");
    MU_ASSERT(FArray_get_pointer(array, 1) == val2, "Wrong second value.");

    return NULL;
}

char *test_remove() {
    int *val_check = FArray_remove_pointer(array, 0);

    MU_ASSERT(val_check != NULL, "Should not get NULL.");
    MU_ASSERT(*val_check == *val1, "Should get the first value.");
    MU_ASSERT(FArray_get_pointer(array, 0) == NULL, "Should be gone.");
    FArray_free(val_check);

    val_check = FArray_remove_pointer(array, 1);
    MU_ASSERT(val_check != NULL, "Should not get NULL.");
    MU_ASSERT(*val_check == *val2, "Should get the second value.");
    MU_ASSERT(FArray_get_pointer(array, 1) == NULL, "Should be gone.");
    FArray_free(val_check);

    return NULL;
}

char *test_expand() {
    int old_max = array->max;
    MU_ASSERT(FArray_expand(array) == 0, "Error on array expanding");
    MU_ASSERT(array->max == (size_t)(old_max * array->expand_factor + 1),
              "Wrong size after expand.");

    MU_ASSERT(FArray_contract(array) == 0, "Failed to contract");
    MU_ASSERT(array->max == array->end, "Array contraction failed.");
    MU_ASSERT(FArray_contract(array) == 0, "Failed to contract");
    MU_ASSERT(array->max == array->end, "Array contraction failed.");

    return NULL;
}

char *test_push_pop() {
    int i = 0;
    for (i = 0; i < 1000; i++) {
        int *val = FArray_new(array);
        *val = i * 333;
        FArray_push_pointer(array, val);
    }

    LOG_DEBUG("%zu", array->max);
    MU_ASSERT(array->max == 1064, "Wrong max size.");

    for (i = 999; i >= 0; i--) {
        int *val = FArray_pop_pointer(array);
        MU_ASSERT(val != NULL, "Should't get a NULL");
        MU_ASSERT(*val == i * 333, "Wrong value.");
        FArray_free(val);
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

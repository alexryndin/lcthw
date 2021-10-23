#include "array.h"
#include "dbg.h"

Array *Array_create(size_t element_size, size_t initial_max) {
    LOG_DEBUG("JOPA");
    Array *ret = NULL;
    ret = calloc(1, sizeof(Array));
    CHECK_MEM(ret);
    ret->end = 0;
    ret->max = initial_max;
    ret->element_size = element_size;
    ret->expand_factor = DEFAULT_EXPAND_FACTOR;

    ret->contents = calloc(initial_max, sizeof(void *));
    CHECK_MEM(ret->contents);

    return ret;
error:
    if (ret != NULL) {
        if (ret->contents != NULL) {
            free(ret->contents);
        }
        free(ret);
    }
    return NULL;
}

void Array_destroy(Array *array) {
    CHECK(array != NULL, "Invalid array.");
    free(array->contents);
    free(array);

error:
    return;
}

void Array_clear(Array *array) {
    CHECK(array != NULL, "Invalid array.");

    for (int i = 0; i < array->end; i++) {
        free(array->contents[i]);
        array->contents[i] = NULL;
    }

error:
    return;
}

int Array_expand(Array *array) {
    CHECK(array != NULL, "Invalid array.");
    size_t new_size = Array_max(array) * array->expand_factor + 1;
    CHECK(new_size > array->max && new_size > 0, "Bad new array size.");
    void *contents = realloc(array->contents, new_size * sizeof(void *));
    CHECK_MEM(contents);

    memset((contents + array->max * sizeof(void *)), 0,
           (new_size - array->max) * sizeof(void *));

    array->max = new_size;
    array->contents = contents;

    return 0;
error:
    return -1;
}

// int Array_contract(Array *array);

int Array_push(Array *array, void *el) {
    CHECK(array != NULL, "Invalid array.");

    if (Array_len(array) == Array_max(array)) {
        CHECK(Array_expand(array) == 0, "Couldn't expand array");
    }

    Array_inc(array);
    Array_last(array) = el;

error:
    return -1;
}

void *Array_pop(Array *array) {
    CHECK(array != NULL, "Invalid array.");
    CHECK(Array_len(array) > 0, "Empty array.");
    void *ret = Array_last(array);
    Array_last(array) = NULL;
    Array_dec(array);

    return ret;
error:
    return NULL;
}

void Array_clear_destroy(Array *array) {
    CHECK(array != NULL, "Invalid array.");

    for (int i = 0; i < array->end; i++) {
        free(array->contents[i]);
        array->contents[i] = NULL;
    }
    free(array->contents);
    free(array);

error:
    return;
}

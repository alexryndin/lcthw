#ifndef _array_h
#define _array_h
#include "dbg.h"
#include <stdlib.h>

typedef struct Array {
    size_t end;
    size_t max;
    size_t element_size;
    double expand_factor;
    void **contents;
} Array;

Array *Array_create(size_t element_size, size_t initial_max);

void Array_destroy(Array *array);

void Array_clear(Array *array);

int Array_expand(Array *array);

// int Array_contract(Array *array);

int Array_push(Array *array, void *el);

void *Array_pop(Array *array);

void Array_clear_destroy(Array *array);

#define Array_last(A) ((A)->contents[(A)->end - 1])
#define Array_first(A) ((A)->contents[0])
#define Array_len(A) ((A)->end)
#define Array_count(A) Array_end(A)
#define Array_max(A) ((A)->max)
#define Array_inc(A) ((A)->end++)
#define Array_dec(A) ((A)->end--)

#define DEFAULT_EXPAND_FACTOR 1.5;

static inline void Array_set(Array *array, size_t i, void *el) {
    CHECK(i < array->max, "array attempt to set past max.");
    if (i >= array->end)
        array->end = i+1;
    array->contents[i] = el;
error:
    return;
}

static inline void *Array_get(Array *array, size_t i) {
    CHECK(i < array->max, "array attempt to get past max");
    return array->contents[i];
error:
    return NULL;
}

static inline void *Array_remove(Array *array, size_t i) {
    CHECK(i < array->max, "array attempt to get past max");
    void *el = array->contents[i];

    array->contents[i] = NULL;

    return el;
error:
    return NULL;
}

static inline void *Array_new(Array *array) {
    CHECK(array->element_size > 0, "Can't use Array_new on 0 size arrays.");

    return calloc(1, array->element_size);

error:
    return NULL;
}

#define Array_free(E) free((E))

#endif

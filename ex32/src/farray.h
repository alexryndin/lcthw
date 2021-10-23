#ifndef _farray_h
#define _farray_h
#include "dbg.h"
#include <stdlib.h>

typedef struct FArray {
    size_t end;
    size_t max;
    size_t element_size;
    size_t storage_size;
    double expand_factor;
    char *contents;
} FArray;

FArray *FArray_create(size_t element_size, size_t storage_size,
                      size_t initial_max);

void FArray_destroy(FArray *array);

void FArray_clear(FArray *array);

int FArray_expand(FArray *array);

int FArray_contract(FArray *array);

int FArray_push_pointer(FArray *array, void *el);

void *FArray_pop_pointer(FArray *array);

void FArray_clear_destroy(FArray *array);

#define FArray_last(A) ((void **)(array->contents))[(A)->end - 1]
#define FArray_index(i) (array->contents + (i)*array->element_size)
#define FArray_first(A) ((A)->contents[0])
#define FArray_len(A) ((A)->end)
#define FArray_count(A) FArray_end(A)
#define FArray_max(A) ((A)->max)
#define FArray_inc(A) ((A)->end++)
#define FArray_dec(A) ((A)->end--)

#define DEFAULT_EXPAND_FACTOR 1.5;

static inline void FArray_set_pointer(FArray *array, size_t i, void *el) {
    CHECK(i < array->max, "array attempt to set past max.");
    CHECK(array->element_size == sizeof(void *),
          "A pointer can only be set in ** arrays, whereas element size is %lu.", array->element_size);
    if (i >= array->end)
        array->end = i + 1;
    ((void **)(array->contents))[i] = el;
error:
    return;
}

static inline void *FArray_get_pointer(FArray *array, size_t i) {
    CHECK(i < array->max, "array attempt to get past max");
    CHECK(array->element_size == sizeof(void *),
          "Only ** arrays can be used to get a pointer from.");
    return ((void **)(array->contents))[i];
error:
    return NULL;
}

static inline void *FArray_remove_pointer(FArray *array, size_t i) {
    CHECK(i < array->max, "array attempt to get past max");
    CHECK(array->element_size == sizeof(void *),
          "Pointer can only be removed from ** arrays");
    void *el = ((void **)(array->contents))[i];

    ((void **)(array->contents))[i] = NULL;

    return el;
error:
    return NULL;
}

static inline void *FArray_new(FArray *array) {
    CHECK(array->element_size > 0, "Can't use FArray_new on 0 size arrays.");

    return calloc(1, array->element_size);

error:
    return NULL;
}

#define FArray_free(E) free((E))

#endif

#ifndef farray_algorithms_h
#define farray_algorithms_h

#include "farray.h"

typedef int (*FArray_compare) (const void *a, const void *b);

int FArray_qsort(FArray *array, FArray_compare cmp);

//int FArray_heapsort(FArray *array, FArray_compare cmp);
//
//int FArray_mergesort(FArray *array, FArray_compare cmp);

#endif

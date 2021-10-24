#include "farray.h"
#include "farray_algorithms.h"
#include <stdlib.h>

int FArray_qsort(FArray *array, FArray_compare cmp) {
    qsort(array->contents, FArray_len(array), sizeof(void *), cmp);
    return 0;
}

// int FArray_heapsort(FArray *array, FArray_compare cmp) {
//     return heapsort(array->contents, FArray_len(array), sizeof(void *), cmp);
// }
// 
// int FArray_mergesort(FArray *array, FArray_compare cmp) {
//     return mergesort(array->contents, FArray_len(array), sizeof(void *), cmp);
// }

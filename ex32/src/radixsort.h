#ifndef radixsort_h
#define radixsort_h
#include <stdint.h>
#include <stddef.h>

void radix_sort(uint32_t *src, uint32_t *dst, size_t n, uint8_t offset);

void radix_sort_4(uint32_t *src, uint32_t *dst, size_t n);

#endif


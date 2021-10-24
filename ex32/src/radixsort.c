#include "dbg.h"
#include <stddef.h>
#include <stdint.h>

#define ByteOf(a, b) (((uint8_t *)&a)[(b)])

void radix_sort(uint32_t *src, uint32_t *dst, size_t n, uint8_t offset) {
    int i = 0;
    uint8_t count[257] = {0};

    for (i = 0; i < n; i++) {
        count[ByteOf(src[i], offset) + 1]++;
    }

    for (i = 1; i < 257; i++) {
        count[i] += count[i - 1];
    }

    for (i = 0; i < n; i++) {
        dst[count[ByteOf(src[i], offset)]++] = src[i];
    }

    return;
}

void radix_sort_4(uint32_t *src, uint32_t *dst, size_t n) {
    radix_sort(src, dst, n, 0);
    radix_sort(dst, src, n, 1);
    radix_sort(src, dst, n, 2);
    radix_sort(dst, src, n, 3);
}

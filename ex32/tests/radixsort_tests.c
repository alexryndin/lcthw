#include "../src//dbg.h"
#include "../src/radixsort.h"
#include "minunit.h"
#include <stdint.h>
#include <time.h>

const uint32_t mask = (uint32_t)(65535 << 16);

char is_sorted(uint32_t *numbers, int n) {
    for (int i = 0; i < n - 1; i++) {
        if (numbers[i] > numbers[i + 1]) {
            return 0;
        }
    }
    return 1;
}

#define ByteOf(a, b) (((uint8_t *)&a)[(b)])

char *test_simple() {
    uint32_t numbers[] = {444444444U, 4164586397, 0, 0, 0,          0, 0, 1, 1,
                          1,          1,          1, 1, 4164586397, 0, 0, 0, 0};
    uint32_t size = sizeof(numbers) / sizeof(uint32_t);
    uint32_t *res = calloc(size, sizeof(uint32_t));

    radix_sort_4(numbers, res, size);

    MU_ASSERT(is_sorted(numbers, size) == 1, "Not sorted.");
    return NULL;
}
char *test_random() {
    uint32_t numbers[100] = {0};
    // LOG_DEBUG("Number is %d offsets are %d %d %d %d", numbers[0],
    // ByteOf(numbers[0], 0), ByteOf(numbers[0], 1), ByteOf(numbers[0], 2),
    // ByteOf(numbers[0], 3));
    uint32_t *res =
        calloc(sizeof(numbers) / sizeof(uint32_t), sizeof(uint32_t));
    uint32_t size = sizeof(numbers) / sizeof(uint32_t);
    uint32_t n = 0;
    for (int i = 0; i < size; i++) {
        n = (rand() & 65535) | ((rand() << 16) & mask);
        numbers[i] = n;
    }
    radix_sort_4(numbers, res, size);

    MU_ASSERT(is_sorted(numbers, sizeof(numbers) / sizeof(uint32_t)) == 1,
              "Not sorted.");

    return NULL;
}

char *all_tests() {
    MU_SUITE_START();
    srand(time(NULL));

    MU_RUN_TEST(test_simple);
    MU_RUN_TEST(test_random);

    return NULL;
}

RUN_TESTS(all_tests);

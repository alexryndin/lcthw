#include "../src/farray_algorithms.h"
#include "minunit.h"

int testcmp(char **a, char **b) { return strcmp(*a, *b); }

FArray *create_words() {
    FArray *result = FArray_create(sizeof(void *), 0, 5);
    char *words[] = {"asdfsdg", "12341", "wefar", "oiijo", "test"};
    int i = 0;

    for (i = 0; i < 5; i++) {
        FArray_push_pointer(result, words[i]);
    }

    return result;
}

int is_sorted(FArray *array) {
    int i = 0;
    for (i = 0; i < FArray_len(array) - 1; i++) {
        if (strcmp(FArray_get_pointer(array, i),
                   FArray_get_pointer(array, i + 1)) > 0) {
            return 0;
        }
    }

    return 1;
}

char *run_sort_test(int (*func)(FArray *, FArray_compare), const char *name) {
    FArray *words = create_words();
    MU_ASSERT(!is_sorted(words), "Words should start not sorted.");
    LOG_DEBUG("--- Testing %s sorting algorithm", name);
    int rc = func(words, (FArray_compare)testcmp);
    MU_ASSERT(rc == 0, "sort failed");
    MU_ASSERT(is_sorted(words), "didn't sort it");

    FArray_destroy(words);

    return NULL;
}

char *test_qsort() { return run_sort_test(FArray_qsort, "qsort"); }
//char *test_heapsort() { return run_sort_test(FArray_heapsort, "heapsort"); }
//char *test_mergesort() { return run_sort_test(FArray_mergesort, "mergesort"); }

char *all_tests() {
    MU_SUITE_START();

    MU_RUN_TEST(test_qsort);
 //   MU_RUN_TEST(test_heapsort);
 //   MU_RUN_TEST(test_mergesort);

    return NULL;
}

RUN_TESTS(all_tests);

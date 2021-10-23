#include "minunit.h"
#include "../src/list_algorithms.h"
#include "../src/dbg.h"
#include <assert.h>
#include <string.h>

// char *values[] = { "XXXX", "1234", "abcd", "xjvef", "NDSS" };
 char *values[] = { "g", "b", "a", "dd", "f", "g", "dd", "ddd" };
 char numbers[] = { -1, 2, 8, 64, -3, 67, -100, -101 };

#define NUM_VALUES 8

List *create_words() {
    int i = 0;
    List *words = List_create();

    for (i = 0; i < NUM_VALUES; i++) {
        List_push(words, values[i]);
    }

    return words;
}

List *create_numbers() {
    int i = 0;
    List *words = List_create();

    for (i = 0; i < NUM_VALUES; i++) {
        char *n = malloc(sizeof(char));
        CHECK_MEM(n);
        *n = numbers[i];
        List_push(words, n);
    }

error:
    return words;
}

int is_sorted(List *words) {
    LIST_FOREACH(words, first, next, cur) {
        if (cur->next && strcmp(cur->value, cur->next->value) > 0) {
            LOG_DEBUG("%s %s", (char *)cur->value, (char *)cur->next->value);
            return 0;
        }
    }
    return 1;
}

int cmp(char *a, char *b) {
    if (*a < *b) return -1;
    if (*a > *b) return 1;
    return 0;
}

int are_numbers_sorted(List *numbers) {
    LIST_FOREACH(numbers, first, next, cur) {
        if (cur->next && cmp(cur->value, cur->next->value) > 0) {
            LOG_DEBUG("%d %d", *(char *)cur->value, *(char *)cur->next->value);
            return 0;
        }
    }
    return 1;
}

char *test_bubble_sort() {
    List *words = create_words();
    int rc = List_bubble_sort(words, (List_compare)strcmp);
    MU_ASSERT(rc == 0, "Bubble sort failed.");
    MU_ASSERT(is_sorted(words), "Words are not sorted after buuble sort.");

    rc = List_bubble_sort(words, (List_compare)strcmp);
    MU_ASSERT(rc == 0, "Bubble sort of already sorted failed.");
    MU_ASSERT(is_sorted(words), "Words should be sorted if already bubble sorted.");

    List_destroy(words);

    words = List_create();
    rc = List_bubble_sort(words, (List_compare)strcmp);

    MU_ASSERT(rc == 0, "Bubble sord failed on empty list.");
    MU_ASSERT(is_sorted(words), "Words should be sorted if empty.");

    List_destroy(words);

    return NULL;
}

char *test_numbers_bubble_sort() {
    List *numbers = create_numbers();
    int rc = List_bubble_sort(numbers, (List_compare)cmp);
    MU_ASSERT(rc == 0, "Bubble sort with numbers failed.");
    MU_ASSERT(are_numbers_sorted(numbers), "Numbers are not sorted after buuble sort.");

    rc = List_bubble_sort(numbers, (List_compare)cmp);
    MU_ASSERT(rc == 0, "Bubble sort of already sorted failed.");
    MU_ASSERT(are_numbers_sorted(numbers), "Numbers should be sorted if already bubble sorted.");

    List_clear_destroy(numbers);

    numbers = List_create();
    rc = List_bubble_sort(numbers, (List_compare)cmp);

    MU_ASSERT(rc == 0, "Bubble sord failed on empty list.");
    MU_ASSERT(are_numbers_sorted(numbers), "Numbers should be sorted if empty.");

    List_clear_destroy(numbers);

    return NULL;
}

char *test_merge_sort() {
    List *words = create_words();
    List *res = List_merge_sort(words, (List_compare)strcmp);
    MU_ASSERT(is_sorted(res), "Words are not sorted after merge sort.");
    List *res2 = List_merge_sort(res, (List_compare)strcmp);
    MU_ASSERT(is_sorted(res2), "Should still be sorted after merge sort.");

    //List_destroy(res);
    List_destroy(res2);
    //List_destroy(words);
    return NULL;
}

char *test_numbers_merge_sort() {
    List *numbers = create_numbers();
    List *res = List_merge_sort(numbers, (List_compare)cmp);
    MU_ASSERT(are_numbers_sorted(numbers), "Numbers are not sorted after merge sort.");
    List *res2 = List_merge_sort(res, (List_compare)cmp);
    MU_ASSERT(are_numbers_sorted(numbers), "Should still be sorted after merge sort.");

    //List_clear_destroy(res);
    List_clear_destroy(res2);
    //List_clear_destroy(numbers);
    return NULL;
}

char *all_tests() {
    MU_SUITE_START();
    LOG_DEBUG("JOPA");
    MU_RUN_TEST(test_bubble_sort);
    MU_RUN_TEST(test_merge_sort);
    MU_RUN_TEST(test_numbers_bubble_sort);
    MU_RUN_TEST(test_numbers_merge_sort);

    return NULL;
}

RUN_TESTS(all_tests);

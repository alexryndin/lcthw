#include "minunit.h"
#include "../src/list.h"

static List *list = NULL;
char test1[] = "test1 data";
char test2[] = "test2 data";
char test3[] = "test3 data";

char *test_create() {
    list = List_create();
    MU_ASSERT(list != NULL, "Failed to create list.");

    return NULL;
}

char *test_destroy() {
    List_clear_destroy(list);
    return NULL;
}

char *test_push_pop() {
    MU_ASSERT(list != NULL, "Invalid list.");
    char *v = malloc(sizeof(test1));
    strcpy(v, test1);
    List_push(list, v);
    MU_ASSERT(List_last(list) == v && !strcmp(List_last(list), test1), "Wrong last value.");

    v = malloc(sizeof(test2));
    strcpy(v, test2);
    List_push(list, v);
    MU_ASSERT(List_last(list) == v && !strcmp(List_last(list), test2), "Wrong last value.");

    v = malloc(sizeof(test3));
    strcpy(v, test3);
    List_push(list, v);
    MU_ASSERT(List_last(list) == v && !strcmp(List_last(list), test3), "Wrong last value.");

    MU_ASSERT(List_len(list) == 3, "Wrong count after push.");

    v = List_pop(list);
    MU_ASSERT(!strcmp(v, test3), "Wrong value on pop.");
    free(v);

    v = List_pop(list);
    MU_ASSERT(!strcmp(v, test2), "Wrong value on pop.");
    free(v);

    v = List_pop(list);
    MU_ASSERT(!strcmp(v, test1), "Wrong value on pop.");
    free(v);

    MU_ASSERT(List_len(list) == 0, "Wrong count after pop.");

    return NULL;
}

char *test_unshift() {
    char *v = malloc(sizeof(test1));
    strcpy(v, test1);
    List_unshift(list, v);
    MU_ASSERT(List_first(list) == v && !strcmp(v, test1), "Wrong first value.");

    v = malloc(sizeof(test2));
    strcpy(v, test2);
    List_unshift(list, v);
    MU_ASSERT(List_first(list) == v && !strcmp(v, test2), "Wrong first value.");

    v = malloc(sizeof(test3));
    strcpy(v, test3);
    List_unshift(list, v);
    MU_ASSERT(List_first(list) == v && !strcmp(List_first(list), test3), "Wrong first value.");

    MU_ASSERT(List_len(list) == 3, "Wrong count after unshift.");

    return NULL;
}

char *test_remove() {
    char *val = List_remove(list, list->first->next);
    MU_ASSERT(!strcmp(val, test2), "Wrong removed element.");
    MU_ASSERT(List_len(list) == 2, "Wrong count after remove.");
    MU_ASSERT(!(strcmp(List_first(list), test3)), "Wrong first value after remove.");
    MU_ASSERT(!(strcmp(List_last(list), test1)), "Wrong last value after remove.");
    free(val);

    return NULL;
}

char *test_shift() {
    MU_ASSERT(List_len(list) == 2, "Wrong count before shift.");
    char *val = List_shift(list);
    MU_ASSERT(!(strcmp(val, test3)), "Wrong value on shift.");
    free(val);
    val = List_shift(list);
    MU_ASSERT(!(strcmp(val, test1)), "Wrong value on shift.");
    MU_ASSERT(List_len(list) == 0, "Wrong count after shift.");
    free(val);

    return NULL;
}

char *all_test() {
    MU_SUITE_START();

    MU_RUN_TEST(test_create);
    MU_RUN_TEST(test_push_pop);
    MU_RUN_TEST(test_unshift);
    MU_RUN_TEST(test_remove);
    MU_RUN_TEST(test_shift);
    MU_RUN_TEST(test_unshift);
    MU_RUN_TEST(test_destroy);

    return NULL;
}

RUN_TESTS(all_test);

#include "minunit.h"
#include <assert.h>
#include <bstrlib/bstrlib.h>
#include <string.h>
#include <tstree.h>

char *valueA = "VALUEA";
char *valueB = "VALUEB";
char *value2 = "VALUE2";
char *value4 = "VALUE4";
char *reverse = "VALUER";
int traverse_count = 0;

struct tagbstring test1 = bsStatic("TEST");
struct tagbstring test2 = bsStatic("TEST2");
struct tagbstring test3 = bsStatic("TSET");

struct tagbstring test4 = bsStatic("T");

void TSTree_traverse_test_cb(void *value, void *data) {
    assert(value != NULL && "Should not get NULL value.");
    assert(data == valueA && "Expecting valueA as the data.");
    traverse_count++;
}


char *test_simple() {
    TSTree *tree = NULL;
    tree = TSTree_create();
    MU_ASSERT(tree != NULL, "Failed to insert into tst.");
    TSTreeNode *node = NULL;
    node = TSTree_insert(tree, bdata(&test1), blength(&test1), valueA);
    MU_ASSERT(node != NULL, "Failed to insert into tst.");

    node = TSTree_insert(tree, bdata(&test2), blength(&test2), value2);
    MU_ASSERT(node != NULL, "Failed to insert into tst with second name.");

    node = TSTree_insert(tree, bdata(&test3), blength(&test3), reverse);
    MU_ASSERT(node != NULL, "Failed to insert into tst with reverse name.");

    node = TSTree_insert(tree, bdata(&test4), blength(&test4), value4);
    MU_ASSERT(node != NULL, "Failed to insert into tst with second name.");

    // tst returns the last one inserted
    void *res = TSTree_search(tree, bdata(&test1), blength(&test1));
    MU_ASSERT(res == valueA,
            "Got the wrong value back, should get A not B.");

    // tst does not find if not exact
    res = TSTree_search(tree, "TESTNO", strlen("TESTNO"));
    MU_ASSERT(res == NULL, "Should not find anything.");

    res = TSTree_search_prefix(tree, bdata(&test1), blength(&test1));
    LOG_DEBUG("result: %p, expected: %p", res, valueA);
    MU_ASSERT(res == valueA, "Got wrong valueA by prefix.");

    res = TSTree_search_prefix(tree, bdata(&test1), 1);
    LOG_DEBUG("result: %p, expected: %p", res, valueA);
    MU_ASSERT(res == value4, "Got wrong value4 for prefix of 1.");

    res = TSTree_search_prefix(tree, "TE", strlen("TE"));
    MU_ASSERT(res != NULL, "Should find for short prefix.");

    res = TSTree_search_prefix(tree, "TE--", strlen("TE--"));
    MU_ASSERT(res != NULL, "Should find for partial prefix.");

    traverse_count = 0;
    TSTree_traverse(tree, TSTree_traverse_test_cb, valueA);
    LOG_DEBUG("traverse count is: %d", traverse_count);
    MU_ASSERT(traverse_count == 4, "Didn't find 4 keys.");

    TSTree_destroy(tree);

    return NULL;
}

char *all_tests() {
    MU_SUITE_START();

    MU_RUN_TEST(test_simple);

    return NULL;
}

RUN_TESTS(all_tests);

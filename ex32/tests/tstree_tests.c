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
    MU_ASSERT(res == valueA, "Got the wrong value back, should get A not B.");

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

char *test_random_manual() {
    traverse_count = 0;

    struct tagbstring key1 = bsStatic("a");
    char *value1 = "val1";
    struct tagbstring key2 = bsStatic("aaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    char *value2 = "val2";
    struct tagbstring key3 = bsStatic("b");
    char *value3 = "val3";
    struct tagbstring key4 = bsStatic("bbbbbbbbbbbbbbbbbbbbbb");
    char *value4 = "val4";
    struct tagbstring key5 = bsStatic("sdfsj;sdkhjg;wehpotyqwpeghas;hd");
    char *value5 = "val5";
    struct tagbstring key6 = bsStatic("ab;sdjh;dgshdg;hsa;hg;asdgh;");
    char *value6 = "val6";
    struct tagbstring key7 = bsStatic("s");
    char *value7 = "val7";

    TSTree *tree = NULL;
    tree = TSTree_create();
    MU_ASSERT(tree != NULL, "");
    TSTreeNode *node = NULL;

    node = TSTree_insert(tree, bdata(&key1), blength(&key1), value1);
    MU_ASSERT(node != NULL, "");
    node = TSTree_insert(tree, bdata(&key2), blength(&key2), value2);
    MU_ASSERT(node != NULL, "");
    node = TSTree_insert(tree, bdata(&key3), blength(&key3), value3);
    MU_ASSERT(node != NULL, "");
    node = TSTree_insert(tree, bdata(&key4), blength(&key4), value4);
    MU_ASSERT(node != NULL, "");
    node = TSTree_insert(tree, bdata(&key5), blength(&key5), value5);
    MU_ASSERT(node != NULL, "");
    node = TSTree_insert(tree, bdata(&key6), blength(&key6), value6);
    MU_ASSERT(node != NULL, "");
    node = TSTree_insert(tree, bdata(&key7), blength(&key7), value7);
    MU_ASSERT(node != NULL, "");

    void *res = TSTree_search(tree, bdata(&key1), blength(&key1));
    MU_ASSERT(res == value1, "");
    res = TSTree_search(tree, bdata(&key2), blength(&key2));
    MU_ASSERT(res == value2, "");
    res = TSTree_search(tree, bdata(&key3), blength(&key3));
    MU_ASSERT(res == value3, "");
    res = TSTree_search(tree, bdata(&key4), blength(&key4));
    MU_ASSERT(res == value4, "");
    res = TSTree_search(tree, bdata(&key5), blength(&key5));
    MU_ASSERT(res == value5, "");
    res = TSTree_search(tree, bdata(&key6), blength(&key6));
    MU_ASSERT(res == value6, "");
    res = TSTree_search(tree, bdata(&key7), blength(&key7));
    MU_ASSERT(res == value7, "");

    res = TSTree_search_prefix(tree, "a", strlen("a"));
    MU_ASSERT(res == value1, "Should find for partial prefix.");

    res = TSTree_search_prefix(tree, "a*", strlen("a*"));
    MU_ASSERT(res == value1, "Should find for partial prefix.");

    res = TSTree_search_prefix(tree, "asdfsdfsdf*", strlen("asdfsdfsdf*"));
    MU_ASSERT(res == value1, "Should find for partial prefix.");

    res = TSTree_search_prefix(tree, "sz", strlen("sz"));
    MU_ASSERT(res == value7, "Should find for partial prefix.");

    res = TSTree_search_prefix(tree, "sd", strlen("sd"));
    MU_ASSERT(res == value5, "Should find for partial prefix.");

    res = TSTree_search_prefix(tree, "abc", strlen("abc"));
    LOG_DEBUG("%s", (char *)res);
    MU_ASSERT(res == value1, "Should find for partial prefix.");

    res = TSTree_search_prefix(tree, "ab;", strlen("abc"));
    MU_ASSERT(res == value6, "Should find for partial prefix.");

    res = TSTree_search_prefix(tree, "bch", strlen("bch"));
    MU_ASSERT(res == value3, "Should find for partial prefix.");

    traverse_count = 0;
    TSTree_traverse(tree, TSTree_traverse_test_cb, valueA);
    LOG_DEBUG("traverse count is: %d", traverse_count);
    MU_ASSERT(traverse_count == 7, "Didn't find 7 keys.");

    TSTree_destroy(tree);

    return NULL;
}

char *test_partial() {
    traverse_count = 0;

    struct tagbstring key1 = bsStatic("a");
    char *value1 = "val1";

    TSTree *tree = NULL;
    tree = TSTree_create();
    MU_ASSERT(tree != NULL, "");
    TSTreeNode *node = NULL;

    node = TSTree_insert(tree, bdata(&key1), blength(&key1), value1);
    MU_ASSERT(node != NULL, "");

    char *res = NULL;

    res = TSTree_search_prefix(tree, "/", strlen("/"));
    MU_ASSERT(res == NULL, "Should not find for partial prefix.");

    TSTree_destroy(tree);

    return NULL;
}

char *all_tests() {
    MU_SUITE_START();

    MU_RUN_TEST(test_simple);
    MU_RUN_TEST(test_random_manual);
    MU_RUN_TEST(test_partial);

    return NULL;
}

RUN_TESTS(all_tests);

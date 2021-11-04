#include "minunit.h"
#include <assert.h>
#include <bstree.h>
#include <bstrlib/bstrlib.h>
#include <stdlib.h>
#include <time.h>

static int traverse_called = 0;

static int traverse_good_cb(BSTreeNode *node) {
    LOG_DEBUG("KEY: %s", bdata((bstring)node->key));
    traverse_called++;
    return 0;
}

static int traverse_fail_cb(BSTreeNode *node) {
    LOG_DEBUG("KEY: %s", bdata((bstring)node->key));
    traverse_called++;

    if (traverse_called == 2) {
        return 1;
    } else {
        return 0;
    }
    return -1;
}

char *test_simple() {
    int rc = 0;
    BSTree *t = NULL;
    struct tagbstring test1 = bsStatic("test data 1");
    struct tagbstring test2 = bsStatic("test data 2");
    struct tagbstring test3 = bsStatic("test data 3");
    struct tagbstring expect1 = bsStatic("THE VALUE 1");
    struct tagbstring expect2 = bsStatic("THE VALUE 2");
    struct tagbstring expect3 = bsStatic("THE VALUE 3");
    t = BSTree_create(NULL);
    MU_ASSERT(t != NULL, "Failed to create tree.");
    MU_ASSERT(t->compare != NULL, "Failed to create tree.");

    rc = BSTree_set(t, &test1, &expect1);
    MU_ASSERT(rc == 0, "Failed to set &test1");
    bstring result = BSTree_get(t, &test1);
    LOG_DEBUG("result is %s", result->data);
    MU_ASSERT(result == &expect1, "Wrong value for test1.");

    rc = BSTree_set(t, &test2, &expect2);
    MU_ASSERT(rc == 0, "Failed to set &test2");
    result = BSTree_get(t, &test2);
    MU_ASSERT(result == &expect2, "Wrong value for test2.");

    rc = BSTree_set(t, &test3, &expect3);
    MU_ASSERT(rc == 0, "Failed to set &test3");
    result = BSTree_get(t, &test3);
    MU_ASSERT(result == &expect3, "Wrong value for test3.");

    rc = BSTree_traverse(t, traverse_good_cb);
    MU_ASSERT(rc == 0, "Failed to traverse.");
    MU_ASSERT(traverse_called == 3, "Wrong count traverse.");

    traverse_called = 0;
    rc = BSTree_traverse(t, traverse_fail_cb);
    MU_ASSERT(rc == -1, "Failed to traverse.");
    MU_ASSERT(traverse_called == 2, "Wrong count traverse for fail.");

    bstring deleted = (bstring)BSTree_delete(t, &test1, 0);
    MU_ASSERT(deleted != NULL, "Got NULL on delete.");
    MU_ASSERT(deleted == &expect1, "Should get test1.");

    result = BSTree_get(t, &test1);
    MU_ASSERT(result == NULL, "Should delete.");
    rc = BSTree_traverse(t, traverse_good_cb);

    deleted = (bstring)BSTree_delete(t, &test2, 0);
    MU_ASSERT(deleted != NULL, "Got NULL on delete.");
    MU_ASSERT(deleted == &expect2, "Should get test2.");
    rc = BSTree_traverse(t, traverse_good_cb);

    result = BSTree_get(t, &test2);
    MU_ASSERT(result == NULL, "Should delete.");

    deleted = (bstring)BSTree_delete(t, &test3, 0);
    MU_ASSERT(deleted != NULL, "Got NULL on delete.");
    MU_ASSERT(deleted == &expect3, "Should get test3.");
    rc = BSTree_traverse(t, traverse_good_cb);

    result = BSTree_get(t, &test3);
    MU_ASSERT(result == NULL, "Should delete.");

    BSTree_destroy(t, 0, 0);
    return NULL;
}

char *test_simple_insert_delete() {
    int rc = 0;
    BSTree *t = NULL;
    struct tagbstring test1 = bsStatic("test data 1");
    struct tagbstring test2 = bsStatic("test data 2");
    struct tagbstring test3 = bsStatic("test data 3");
    struct tagbstring expect1 = bsStatic("THE VALUE 1");
    struct tagbstring expect2 = bsStatic("THE VALUE 2");
    struct tagbstring expect3 = bsStatic("THE VALUE 3");
    t = BSTree_create(NULL);
    MU_ASSERT(t != NULL, "Failed to create tree.");
    MU_ASSERT(t->compare != NULL, "Failed to create tree.");

    rc = BSTree_set(t, &test2, &expect2);
    MU_ASSERT(rc == 0, "Failed to set &test2");
    bstring result = BSTree_get(t, &test2);
    MU_ASSERT(result == &expect2, "Wrong value for test2.");

    rc = BSTree_set(t, &test1, &expect1);
    MU_ASSERT(rc == 0, "Failed to set &test1");
    result = BSTree_get(t, &test1);
    MU_ASSERT(result == &expect1, "Wrong value for test1.");

    rc = BSTree_set(t, &test3, &expect3);
    MU_ASSERT(rc == 0, "Failed to set &test3");
    result = BSTree_get(t, &test3);
    MU_ASSERT(result == &expect3, "Wrong value for test3.");

    bstring deleted = (bstring)BSTree_delete(t, &test2, 0);
    MU_ASSERT(deleted != NULL, "Got NULL on delete.");
    MU_ASSERT(deleted == &expect2, "Should get test2.");

    result = BSTree_get(t, &test2);
    MU_ASSERT(result == NULL, "Should delete.");

    deleted = (bstring)BSTree_delete(t, &test2, 0);
    MU_ASSERT(deleted != NULL, "Got NULL on delete.");
    MU_ASSERT(deleted == &expect2, "Should get test2.");

    result = BSTree_get(t, &test2);
    MU_ASSERT(result == NULL, "Should delete.");

    deleted = (bstring)BSTree_delete(t, &test3, 0);
    MU_ASSERT(deleted != NULL, "Got NULL on delete.");
    MU_ASSERT(deleted == &expect3, "Should get test3.");
    rc = BSTree_traverse(t, traverse_good_cb);

    result = BSTree_get(t, &test3);
    MU_ASSERT(result == NULL, "Should delete.");

    BSTree_destroy(t, 0, 0);
    return NULL;
}
char *test_fuzzing() {
    BSTree *store = BSTree_create(NULL);
    int i = 0;
    int j = 0;
    bstring numbers[100] = {NULL};
    bstring data[100] = {NULL};
    srand((unsigned int)time(NULL));

    for (i = 0; i < 100; i++) {
        int num = rand();
        numbers[i] = bformat("%d", num);
        data[i] = bformat("data %d", num);
        BSTree_set(store, numbers[i], data[i]);
    }

    for (i = 0; i < 100; i++) {
        bstring value = BSTree_delete(store, numbers[i], 0);
        MU_ASSERT(value == data[i], "Failed to delete the right number.");
        MU_ASSERT(BSTree_delete(store, numbers[i], 0) == NULL,
                  "Should get nothing.");

        for (j = i + 1; j < 100; j++) {
            bstring value = BSTree_get(store, numbers[j]);
            LOG_DEBUG("i %d, j %d", i, j);
            MU_ASSERT(value == data[j], "Failed to delete the right number.");
        }

        bdestroy(value);
        bdestroy(numbers[i]);
    }

    BSTree_destroy(store, 0, 0);

    return NULL;
}

char *all_tests() {
    MU_SUITE_START();

    MU_RUN_TEST(test_simple);
  //  MU_RUN_TEST(test_simple_insert_delete);
    MU_RUN_TEST(test_fuzzing);

    return NULL;
}

RUN_TESTS(all_tests);

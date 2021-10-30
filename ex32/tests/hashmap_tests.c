#include <bstrlib/bstrlib.h>
#include <hashmap.h>
#include "minunit.h"
#include <assert.h>

Hashmap *map = NULL;
static int traverse_called = 0;

struct tagbstring test1 = bsStatic("test data 1");
struct tagbstring test2 = bsStatic("test data 2");
struct tagbstring test3 = bsStatic("test data 3");
struct tagbstring expect1 = bsStatic("THE VALUE 1");
struct tagbstring expect2 = bsStatic("THE VALUE 2");
struct tagbstring expect3 = bsStatic("THE VALUE 3");

static int traverse_good_cb(HashmapNode *node) {
    LOG_DEBUG("KEY: %s", bdata((bstring)node->key));
    traverse_called++;
    return 0;
}

static int traverse_fail_cb(HashmapNode *node) {
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
    map = Hashmap_create(NULL, NULL);
    MU_ASSERT(map != NULL, "Failed to create map.");
    MU_ASSERT(map != NULL, "Failed to create map.");
    MU_ASSERT(map->hash != NULL, "Failed to create map.");
    MU_ASSERT(map->compare != NULL, "Failed to create map.");

    rc = Hashmap_set(map, &test1, &expect1);
    MU_ASSERT(rc == 0, "Failed to set &test1");
    bstring result = Hashmap_get(map, &test1);
    MU_ASSERT(result == &expect1, "Wrong value for test1.");

    rc = Hashmap_set(map, &test2, &expect2);
    MU_ASSERT(rc == 0, "Failed to set &test2");
    result = Hashmap_get(map, &test2);
    MU_ASSERT(result == &expect2, "Wrong value for test2.");

    rc = Hashmap_set(map, &test3, &expect3);
    MU_ASSERT(rc == 0, "Failed to set &test3");
    result = Hashmap_get(map, &test3);
    MU_ASSERT(result == &expect3, "Wrong value for test3.");


    rc = Hashmap_traverse(map, traverse_good_cb);
    MU_ASSERT(rc == 0, "Failed to traverse.");
    MU_ASSERT(traverse_called == 3, "Wrong count traverse.");

    traverse_called = 0;
    rc = Hashmap_traverse(map, traverse_fail_cb);
    MU_ASSERT(rc == -1, "Failed to traverse.");
    MU_ASSERT(traverse_called == 2, "Wrong count traverse for fail.");


    bstring deleted = (bstring)Hashmap_delete(map, &test1);
    MU_ASSERT(deleted != NULL, "Got NULL on delete.");
    MU_ASSERT(deleted == &expect1, "Should get test1.");

    result = Hashmap_get(map, &test1);
    MU_ASSERT(result == NULL, "Should delete.");

    deleted = (bstring)Hashmap_delete(map, &test2);
    MU_ASSERT(deleted != NULL, "Got NULL on delete.");
    MU_ASSERT(deleted == &expect2, "Should get test2.");

    result = Hashmap_get(map, &test2);
    MU_ASSERT(result == NULL, "Should delete.");

    deleted = (bstring)Hashmap_delete(map, &test3);
    MU_ASSERT(deleted != NULL, "Got NULL on delete.");
    MU_ASSERT(deleted == &expect3, "Should get test3.");

    result = Hashmap_get(map, &test3);
    MU_ASSERT(result == NULL, "Should delete.");

    Hashmap_destroy(map);
    return NULL;
}

char *test_uint32() {
    uint32_t i = 0;
    int rc = 0;
    uint32_t *key = NULL, *value = NULL;
    Hashmap *map = NULL;
    map = Hashmap_create((Hashmap_compare)uint32_cmp,
                             (Hashmap_hash)uint32_hash);
    MU_ASSERT(map != NULL, "Map creationg failed.");
    for (i = 0; i < 10000; i++) {
        LOG_DEBUG("inserting %d", i);
        key = malloc(sizeof(uint32_t));
        value = malloc(sizeof(uint32_t));
        CHECK_MEM(key);
        CHECK_MEM(value);
        *value = i * 333;
        *key = i;
        rc = Hashmap_set(map, key, value);
        MU_ASSERT(rc == 0, "Failed to insert into map");
    }

    for (i = 0; i < 10000; i++) {
        value = Hashmap_get(map, &i);
        MU_ASSERT(*value == i * 333, "Failed to get from map");
    }

    LOG_DEBUG("number of buckets %zu", map->number_of_buckets);
    LOG_DEBUG("number of elements %zu", map->number_of_elements);
    MU_ASSERT(Hashmap_destroy_with_kv(map) == 0, "Failed to destroy map.");
error:
    return NULL;
}


char *all_tests() {
    MU_SUITE_START();

    MU_RUN_TEST(test_simple);
    MU_RUN_TEST(test_uint32);

    return NULL;
}

RUN_TESTS(all_tests);

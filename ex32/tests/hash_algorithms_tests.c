#include "minunit.h"
#include <bstrlib/bstrlib.h>
#include <farray.h>
#include <hashmap.h>
#include <hashmap_utils.h>

struct tagbstring test1 = bsStatic("test data 1");
struct tagbstring test2 = bsStatic("test data 2");
struct tagbstring test3 = bsStatic("test data 3");

char *test_fnv1a() {
    uint32_t hash = str_hash_fnv1a(&test1);
    MU_ASSERT(hash != 0, "Zero hash.");
    hash = str_hash_fnv1a(&test2);
    MU_ASSERT(hash != 0, "Zero hash.");
    hash = str_hash_fnv1a(&test3);
    MU_ASSERT(hash != 0, "Zero hash.");

    return NULL;
}

char *test_adler32() {
    uint32_t hash = str_hash_adler32(&test1);
    MU_ASSERT(hash != 0, "Zero hash.");
    hash = str_hash_adler32(&test2);
    MU_ASSERT(hash != 0, "Zero hash.");
    hash = str_hash_adler32(&test3);
    MU_ASSERT(hash != 0, "Zero hash.");

    return NULL;
}

char *test_jenkins_hash() {
    uint32_t hash = str_hash(&test1);
    MU_ASSERT(hash != 0, "Zero hash.");
    hash = str_hash(&test2);
    MU_ASSERT(hash != 0, "Zero hash.");
    hash = str_hash(&test3);
    MU_ASSERT(hash != 0, "Zero hash.");

    return NULL;
}
char *test_djb() {
    uint32_t hash = str_hash_djb(&test1);
    MU_ASSERT(hash != 0, "Zero hash.");
    hash = str_hash_djb(&test2);
    MU_ASSERT(hash != 0, "Zero hash.");
    hash = str_hash_djb(&test3);
    MU_ASSERT(hash != 0, "Zero hash.");

    return NULL;
}

#define BUCKETS    100
#define BUFFER_LEN 20
#define NUM_KEYS   (BUCKETS * 1000)

enum { ALGO_FNV1A, ALGO_ADLER32, ALGO_DJB, ALGO_JENKINS };

int gen_keys(FArray *keys, int num_keys) {
    int i = 0;
    FILE *urand = fopen("/dev/urandom", "r");
    CHECK(urand != NULL, "Failed to open /dev/urandom");

    struct bStream *stream = bsopen((bNread)fread, urand);
    CHECK(stream != NULL, "Failed to open /dev/urandom");

    bstring key = bfromcstr("");
    int rc = 0;

    for (i = 0; i < num_keys; i++) {
        rc = bsread(key, stream, BUFFER_LEN);
        CHECK(rc == BSTR_OK, "Failed to read from /dev/urandom.");

        FArray_push_pointer(keys, bstrcpy(key));
    }

    bsclose(stream);
    fclose(urand);
    return 0;

error:
    return -1;
}

void destroy_keys(FArray *keys) {
    int i = 0;
    for (i = 0; i < NUM_KEYS; i++) {
        bdestroy(FArray_get_pointer(keys, i));
    }

    FArray_destroy(keys);
}

void fill_distribution(int *stats, FArray *keys, Hashmap_hash func) {
    size_t i = 0;
    uint32_t hash = 0;

    for (i = 0; i < FArray_len(keys); i++) {
        hash = func(FArray_get_pointer(keys, i));
        stats[hash % BUCKETS] += 1;
    }
}

char *test_distribution() {
    int i = 0;
    int stats[4][BUCKETS] = {{0}};
    FArray *keys = FArray_create(sizeof(void *), 0, NUM_KEYS);

    MU_ASSERT(gen_keys(keys, NUM_KEYS) == 0, "Failed to generate random keys.");

    fill_distribution(stats[ALGO_FNV1A], keys, (Hashmap_hash)str_hash_fnv1a);
    fill_distribution(stats[ALGO_ADLER32], keys,
                      (Hashmap_hash)str_hash_adler32);
    fill_distribution(stats[ALGO_DJB], keys, (Hashmap_hash)str_hash_djb);
    fill_distribution(stats[ALGO_JENKINS], keys, (Hashmap_hash)str_hash);

    fprintf(stdout, "FNV\tA32\tDJB\tJENKINS\n");

    for (i = 0; i < BUCKETS; i++) {
        fprintf(stdout, "%d\t%d\t%d\t%d\n", stats[ALGO_FNV1A][i],
                stats[ALGO_ADLER32][i], stats[ALGO_DJB][i], stats[ALGO_JENKINS][i]);
    }

    destroy_keys(keys);
    return NULL;
}

char *all_tests() {
    MU_SUITE_START();
    MU_RUN_TEST(test_fnv1a);
    MU_RUN_TEST(test_adler32);
    MU_RUN_TEST(test_djb);
    MU_RUN_TEST(test_jenkins_hash);
    MU_RUN_TEST(test_distribution);

    return NULL;
}

RUN_TESTS(all_tests);

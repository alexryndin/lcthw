#include "hashmap_utils.h"
#include <bstrlib/bstrlib.h>
#include <dbg.h>
#include <stdint.h>

typedef int (*Hashmap_compare)(void *a, void *b);
typedef uint32_t (*Hashmap_hash)(void *key);

int str_cmp(bstring b1, bstring b2) {
    int res = bstrcmp(b1, b2);
    if (res == SHRT_MIN)
        return -2;
    if (res < 0)
        return -1;
    if (res > 0)
        return 1;
    return 0;
}

int uint32_cmp(uint32_t *a, uint32_t *b) {
    if (*a < *b)
        return -1;
    if (*a > *b)
        return 1;
    return 0;
}

uint32_t str_hash_fnv1a(const bstring key) {
    if (blength(key) < 0) {
        LOG_ERR("Cannot get hash of a string of negative length.");
        return 0;
    }
    uint32_t ret = 0;
    FNV1A_HASH(ret, (uint32_t)blength(key), key, bchar);
    return ret;
}

uint32_t str_hash_adler32(const bstring key) {
    if (blength(key) < 0) {
        LOG_ERR("Cannot get hash of a string of negative length.");
        return 0;
    }
    uint32_t ret = 0;
    ADLER32_HASH(ret, (uint32_t)blength(key), key, bchar);
    return ret;
}

uint32_t str_hash_djb(const bstring key) {
    if (blength(key) < 0) {
        LOG_ERR("Cannot get hash of a string of negative length.");
        return 0;
    }
    uint32_t ret = 0;
    DJB_HASH(ret, (uint32_t)blength(key), key, bchar);
    return ret;
}

uint32_t str_hash(const bstring key) {
    if (blength(key) < 0) {
        LOG_ERR("Cannot get hash of a string of negative length.");
        return 0;
    }
    uint32_t ret = 0;

    JENKINS_HASH(ret, (uint32_t)blength(key), key, bchar);
    return ret;
}

uint32_t uint32_hash(uint32_t *num) {
    uint32_t ret = 0;

    JENKINS_HASH(ret, 4, num, ByteOf);
    return ret;
}

#include "hashmap_utils.h"
#include <dbg.h>
#include <bstrlib/bstrlib.h>
#include <stdint.h>

typedef int (*Hashmap_compare)(void *a, void *b);
typedef uint32_t (*Hashmap_hash)(void *key);

int str_cmp(bstring b1, bstring b2) { return bstrcmp(b1, b2); }

int uint32_cmp(uint32_t *a, uint32_t *b) {
    if (*a < *b)
        return -1;
    if (*a > *b)
        return 1;
    return 0;
}

uint32_t str_hash(const bstring key) {
    if (blength(key) < 0) {
        LOG_ERR("Cannot get hash of a string of negative length.");
        return 0;
    }
    uint32_t ret, i;

    JENKINS_HASH(ret, (uint32_t)blength(key), key, bchar);
    return ret;
}

uint32_t uint32_hash(uint32_t *num) {
    uint32_t ret, i;

    JENKINS_HASH(ret, 4, num, ByteOf);
    return ret;
}

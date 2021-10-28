#include "../external/bstrlib/bstrlib.h"
#include "hashmap_utils.h"
#include <stdint.h>

typedef int (*Hashmap_compare)(void *a, void *b);
typedef uint32_t (*Hashmap_hash)(void *key);

int str_cmp(bstring b1, bstring b2) { return bstrcmp(b1, b2); }

int uint32_cmp(uint32_t *a, uint32_t *b) {
    if (*a < *b) return -1;
    if (*a > *b) return 1;
    return 0;
}

uint32_t str_hash(const bstring key) {
    uint32_t ret, i;

    for (ret = i = 0; i < blength(key); i++) {
        ret += bchar(key, i);
        ret += (ret << 10);
        ret ^= (ret >> 6);
    }

    ret += (ret << 3);
    ret ^= (ret >> 11);
    ret += (ret << 15);
    return ret;
}

uint32_t uint32_hash(uint32_t *num) {
    uint32_t ret, i;

    for (ret = i = 0; i < 4; i++) {
        ret += ByteOf(num, i);
        ret += (ret << 10);
        ret ^= (ret >> 6);
    }

    ret += (ret << 3);
    ret ^= (ret >> 11);
    ret += (ret << 15);
    return ret;
}

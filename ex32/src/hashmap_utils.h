#ifndef hashmap_utils_h
#define hashmap_utils_h

#include <bstrlib/bstrlib.h>
#include <stdint.h>

#define ByteOf(a, b) (((uint8_t *)a)[(b)])

// setting taken from
// http://www.isthe.com/chongo/tech/comp/fnv/index.html#FNV-param
static const uint32_t FNV_PRIME = 16777619;
static const uint32_t FNV_OFFSET_BASIS = 2166136261;
#define FNV1A_HASH(ret, len, value, accessor) \
    ret = FNV_OFFSET_BASIS;                   \
    do {                                      \
        for (uint32_t i = 0; i < (len); i++) {     \
            ret += accessor(value, i);        \
            ret *= FNV_PRIME;                 \
        }                                     \
    } while (0)

static const int MOD_ADLER = 65521;
#define ADLER32_HASH(ret, len, value, accessor)   \
    uint32_t a = 1, b = 0;                        \
    uint32_t i = 0;                               \
    for (ret = i = 0; i < (len); i++) {           \
        a = (a + accessor(value, i)) % MOD_ADLER; \
        b = (b + a) % MOD_ADLER;                  \
    }                                             \
                                                  \
    ret = (b << 16) | a

#define DJB_HASH(ret, len, value, accessor)                \
    ret = 5381;                                            \
    do {                                                   \
        for (uint32_t i = 0; i < len; i++) {               \
            ret = ((ret << 5) + ret) + accessor(value, i); \
        }                                                  \
    } while (0)

#define JENKINS_HASH(ret, len, value, accessor) \
    for (uint32_t i = 0; i < (len); i++) {      \
        ret += accessor(value, i);              \
        ret += (ret << 10);                     \
        ret ^= (ret >> 6);                      \
    }                                           \
    ret += (ret << 3);                          \
    ret ^= (ret >> 11);                         \
    ret += (ret << 15)

typedef int (*Hashmap_compare)(void *a, void *b);
typedef uint32_t (*Hashmap_hash)(void *key);

int str_cmp(bstring b1, bstring b2);
uint32_t str_hash(const bstring key);
uint32_t str_hash_fnv1a(const bstring key);
uint32_t str_hash_adler32(const bstring key);
uint32_t str_hash_djb(const bstring key);

int uint32_cmp(uint32_t *a, uint32_t *b);
uint32_t uint32_hash(uint32_t *key);

#endif

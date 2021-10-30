#include "hashmap.h"
#include "../external/bstrlib/bstrlib.h"
#include "dbg.h"
#include "farray.h"
#include "hashmap_utils.h"

Hashmap *_Hashmap_create(Hashmap_compare compare, Hashmap_hash hash,
                         size_t number_of_buckets) {
    size_t i = 0;
    FArray *new_bucket = NULL;
    Hashmap *ret = calloc(1, sizeof(Hashmap));
    CHECK_MEM(ret);

    ret->compare = compare != NULL ? compare : (Hashmap_compare)str_cmp;
    ret->hash = hash != NULL ? hash : (Hashmap_hash)str_hash;
    ret->number_of_buckets = number_of_buckets;
    ret->realloc_factor = HASHMAP_DEFAULT_REALLOC_FACTOR;
    ret->expand_factor = DEFAULT_EXPAND_FACTOR;
    ret->buckets =
        FArray_create(sizeof(void *), sizeof(FArray), ret->number_of_buckets);
    CHECK(ret->buckets != NULL, "Couldn't initialize buckets.");

    CHECK(ret->expand_factor > 1, "Couldn't initialize map.");
    CHECK(ret->realloc_factor > 1, "Couldn't initialize map.");

    for (i = 0; i < ret->number_of_buckets; i++) {
        new_bucket = FArray_create(sizeof(HashmapNode), 0, 5);
        CHECK(new_bucket != NULL, "Couldn't initialize bucket.");
        FArray_set_pointer(ret->buckets, i, new_bucket);
    }

    return ret;
error:
    if (ret != NULL) {
        if (ret->buckets != NULL) {
            for (i = 0; i < ret->number_of_buckets; i++) {
                new_bucket = FArray_get_pointer(ret->buckets, i);
                if (new_bucket != NULL) {
                    FArray_destroy(new_bucket);
                }
            }
            FArray_destroy(ret->buckets);
        }
        free(ret);
    }
    return NULL;
}
Hashmap *Hashmap_create(Hashmap_compare compare, Hashmap_hash hash) {
    return _Hashmap_create(compare, hash, DEFAULT_NUMBER_OF_BUCKETS);
}

int Hashmap_destroy_with_kv(Hashmap *map) {
    int rc = 0;
    size_t i = 0, j = 0;
    HashmapNode *node = NULL;
    FArray *bucket = NULL;
    if (FArray_len(map->buckets) != map->number_of_buckets)
        LOG_ERR("number of buckets != len of buckets array.");
    for (i = 0; i < map->number_of_buckets; i++) {
        bucket = FArray_get_pointer(map->buckets, i);
        CHECK(bucket != NULL, "Couldn't find bucket.");
        for (j = 0; j < FArray_len(bucket); j++) {
            node = ((HashmapNode *)FArray_get(bucket, j));

            CHECK(node != NULL, "Couldn't get bucket node.");

            if (node->key != NULL) {
                free(node->key);
                node->key = NULL;
            }
            if (node->data != NULL) {
                free(node->data);
                node->data = NULL;
            }
        }
        FArray_destroy(bucket);
    }
    FArray_destroy(map->buckets);
    free(map);
error:
    return rc;
}

void Hashmap_destroy(Hashmap *map) {
    size_t i = 0;
    FArray *bucket = NULL;
    if (FArray_len(map->buckets) != map->number_of_buckets)
        LOG_ERR("number of buckets != len of buckets array.");
    for (i = 0; i < map->number_of_buckets; i++) {
        bucket = FArray_get_pointer(map->buckets, i);
        FArray_destroy(bucket);
    }
    FArray_destroy(map->buckets);
    free(map);
}

//
int Hashmap_realloc(Hashmap *map) {
    HashmapNode *hmn = NULL;
    LOG_DEBUG("Reallocating map, number of buckets %zu, expand factor %f.",
              map->number_of_buckets, map->expand_factor);
    int rc = 0;
    size_t i = 0, j = 0;
    Hashmap *new = NULL;
    CHECK(map != NULL, "Cannot reallocate null map.");
    new =
        _Hashmap_create(map->compare, map->hash,
                        (size_t)(map->expand_factor * map->number_of_buckets));
    CHECK(new != NULL, "Couldn't create map to reallocate old.");

    FArray *bucket = NULL;
    for (i = 0; i < map->number_of_buckets; i++) {
        bucket = FArray_get_pointer(map->buckets, i);
        CHECK(bucket != NULL, "Couldn't find bucket.");
        for (j = 0; j < FArray_len(bucket); j++) {
            hmn = ((HashmapNode *)FArray_get(bucket, j));

            CHECK(hmn != NULL, "Couldn't get bucket node.");

            rc = Hashmap_set(new, hmn->key, hmn->data);
            CHECK(rc == 0, "Couldn't add node to a new map.");
        }
    }

    // new map is created, now we use its guts to expand old map
    // :)
    FArray *swap = new->buckets;
    new->buckets = map->buckets;
    map->buckets = swap;

    size_t swap_n = new->number_of_buckets;
    new->number_of_buckets = map->number_of_buckets;
    map->number_of_buckets = swap_n;

error:
    if (new != NULL) {
        Hashmap_destroy(new);
    }
    return rc;
}

int Hashmap_set(Hashmap *map, void *key, void *data) {
    uint32_t hash = map->hash(key), i = 0;
    HashmapNode *hmn = NULL;
    int rc = 0;
    FArray *bucket =
        FArray_get_pointer(map->buckets, hash % map->number_of_buckets);
    CHECK(bucket != NULL, "Couldn't find bucket.");

    for (i = 0; i < FArray_len(bucket); i++) {
        hmn = ((HashmapNode *)FArray_get(bucket, i));
        CHECK(hmn != NULL, "Couldn't get bucket node.");

        if (hash == hmn->hash && map->compare(key, hmn->key) == 0) {
            LOG_DEBUG("Found existing node, rewriting.");
            hmn->data = data;
            hmn->key = key;
            goto exit;
        }
    }

    rc = FArray_push(bucket,
                     &(HashmapNode){.key = key, .data = data, .hash = hash});
    map->number_of_elements++;
    if (map->number_of_elements >
        map->number_of_buckets * map->realloc_factor) {
        rc = Hashmap_realloc(map);
        CHECK(rc == 0, "Couldn't reallocate map.");
    }

exit:
    return rc;
error:
    rc = -1;
    goto exit;
}

void *Hashmap_get(Hashmap *map, void *key) {
    void *ret = NULL;
    HashmapNode *hmn = NULL;
    uint32_t hash = map->hash(key), i = 0, node_hash = 0;
    FArray *bucket =
        FArray_get_pointer(map->buckets, hash % map->number_of_buckets);
    CHECK(bucket != NULL, "Couldn't find bucket.");

    for (i = 0; i < FArray_len(bucket); i++) {
        hmn = ((HashmapNode *)FArray_get(bucket, i));
        CHECK(hmn != NULL, "Couldn't get bucket node.");

        node_hash = hmn->hash;
        if (hash == node_hash && map->compare(key, hmn->key) == 0) {
            ret = hmn->data;
            goto exit;
        }
    }

exit:
error:
    return ret;
}

int Hashmap_traverse(Hashmap *map, Hashmap_traverse_cb traverse_cb) {
    int rc = 0;
    HashmapNode *hmn = NULL;
    size_t i = 0, j = 0;
    FArray *bucket = NULL;
    for (i = 0; i < map->number_of_buckets; i++) {
        bucket = FArray_get_pointer(map->buckets, i);
        CHECK(bucket != NULL, "Couldn't find bucket.");
        for (j = 0; j < FArray_len(bucket); j++) {
            hmn = ((HashmapNode *)FArray_get(bucket, j));

            CHECK(hmn != NULL, "Couldn't get bucket node.");

            rc = traverse_cb(hmn);
            CHECK(rc == 0, "Callback failed.");
        }
    }

exit:
    return rc;
error:
    rc = -1;
    goto exit;
}

void *Hashmap_delete(Hashmap *map, void *key) {
    void *ret = NULL;
    uint32_t hash = map->hash(key), i = 0, node_hash = 0;
    FArray *bucket =
        FArray_get_pointer(map->buckets, hash % map->number_of_buckets);
    CHECK(bucket != NULL, "Couldn't find bucket.");

    HashmapNode *hmn = NULL, *popped = NULL;
    for (i = 0; i < FArray_len(bucket); i++) {
        hmn = ((HashmapNode *)FArray_get(bucket, i));
        CHECK(hmn != NULL, "Couldn't get bucket node.");

        node_hash = hmn->hash;
        if (hash == node_hash && map->compare(key, hmn->key) == 0) {
            ret = hmn->data;
            if (i < FArray_len(bucket) - 1) {
                popped = (HashmapNode *)FArray_pop(bucket);
                FArray_set(bucket, i, popped);
            } else {
                FArray_dec(bucket);
            }
            goto exit;
        }
    }

exit:
error:
    return ret;
}

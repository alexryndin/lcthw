#include "dbg.h"
#include <stack.h>
#include <stdbool.h>
#include <tstree.h>

typedef struct RET_TWO {
    TSTreeNode *exact;
    TSTreeNode *partial;
} RET_TWO;

typedef struct TSTreeNode {
    char splitchar;
    struct TSTreeNode *low;
    struct TSTreeNode *equal;
    struct TSTreeNode *high;
    void *value;
} TSTreeNode;

TSTree *TSTree_create() {
    TSTree *ret = calloc(1, sizeof(TSTree));
    CHECK_MEM(ret);
    return ret;

error:
    return NULL;
}

static RET_TWO get_node(TSTree *t, const char *key, size_t len, char create,
                            char partial);

void *TSTree_search(TSTree *t, const char *key, size_t len) {
    RET_TWO n = {0};
    CHECK(t != NULL, "null TSTree.");
    CHECK(key != NULL, "null key.");
    CHECK(len > 0, "zero len.");
    n = get_node(t, key, len, false, false);

    if (n.exact != NULL) {
        return n.exact->value;
    }
error:
    return NULL;
}

void *TSTree_search_prefix(TSTree *t, const char *key, size_t len) {
    RET_TWO n = {0};
    CHECK(t != NULL, "null TSTree.");
    CHECK(key != NULL, "null key.");
    CHECK(len > 0, "zero len.");
    n = get_node(t, key, len, false, true);

    while (n.exact != NULL && (n.exact->value == NULL)) {
        n.exact = n.exact->equal;
    }
    if (n.exact == NULL || n.exact->value == NULL) {
        return n.partial != NULL ? n.partial->value : NULL;
    }
    return n.exact->value;

error:
    return NULL;
}

static RET_TWO get_node(TSTree *t, const char *key, size_t len, char create,
                            char partial) {
    RET_TWO ret = {0};
    CHECK(t != NULL, "null TSTree.");
    CHECK(key != NULL, "null key.");
    CHECK(len > 0, "zero len.");

    if (t->root == NULL) {
        if (create) {
            t->root = calloc(1, sizeof(TSTreeNode));
            t->root->splitchar = key[0];
        } else {
            return ret;
        }
    }

    ret.exact = t->root;

    while (len > 0) {
        if (key[0] == ret.exact->splitchar) {
            if (ret.exact->value != NULL) {
                ret.partial = ret.exact;
            }
            if (len == 1) {
                break;
            }
            if (ret.exact->equal == NULL) {
                if (create) {
                    ret.exact->equal = calloc(1, sizeof(TSTreeNode));
                    CHECK_MEM(ret.exact->equal);
                    ret.exact->equal->splitchar = key[1];
                } else {
                    ret.exact = NULL;
                    break;
                }
            }
            ret.exact = ret.exact->equal;
            len--;
            key++;
            continue;
        } else if (key[0] < ret.exact->splitchar) {
            if (ret.exact->low == NULL) {
                if (create) {
                    ret.exact->low = calloc(1, sizeof(TSTreeNode));
                    CHECK_MEM(ret.exact->equal);
                    ret.exact->low->splitchar = key[0];
                } else {
                    ret.exact = NULL;
                    break;
                }
            }
            ret.exact = ret.exact->low;
            continue;
        } else {
            // key[0] > cur->splitchar
            if (ret.exact->high == NULL) {
                if (create) {
                    ret.exact->high = calloc(1, sizeof(TSTreeNode));
                    CHECK_MEM(ret.exact->equal);
                    ret.exact->high->splitchar = key[0];
                } else {
                    ret.exact = NULL;
                    break;
                }
            }
            ret.exact = ret.exact->high;
            continue;
        }
    }
    // fallthrough

error:
    return ret;
}

TSTreeNode *TSTree_insert(TSTree *t, const char *key, size_t len, void *value) {
    RET_TWO n = {0};
    CHECK(t != NULL, "null TSTree.");
    CHECK(key != NULL, "null key.");
    CHECK(len > 0, "zero len.");

    n = get_node(t, key, len, true, false);

    CHECK(n.exact != NULL, "error getting node.");
    CHECK(n.exact->value == NULL, "already exists.");

    n.exact->value = value;

    return n.exact;

error:
    return NULL;
}

void TSTree_traverse(TSTree *t, TSTree_traverse_cb cb, void *data) {
    TSTreeNode *cur = NULL;
    Stack *s = NULL;

    CHECK(t != NULL, "null TSTree.");
    if (t->root == NULL) {
        return;
    }

    cur = t->root;

    s = Stack_create();
    Stack_push(s, NULL);

    while (true) {
        if (cur == NULL) {
            if (Stack_peek(s) == NULL) {
                break;
            } else {
                cur = Stack_pop(s);
            }
        }
        if (cur->value != NULL) {
            cb(cur->value, data);
        }
        if (cur->equal != NULL) {
            Stack_push(s, cur->equal);
        }
        if (cur->high != NULL) {
            Stack_push(s, cur->high);
        }
        cur = cur->low;
    };

error:
    if (s != NULL)
        Stack_destroy(s);
    return;
}

void TSTree_destroy(TSTree *t) {
    TSTreeNode *cur = NULL;
    TSTreeNode *next_cur = NULL;
    Stack *s = NULL;

    CHECK(t != NULL, "null TSTree.");
    if (t->root == NULL) {
        return;
    }

    cur = t->root;

    s = Stack_create();
    Stack_push(s, NULL);

    while (true) {
        if (cur == NULL) {
            if (Stack_peek(s) == NULL) {
                break;
            } else {
                cur = Stack_pop(s);
            }
        }
        if (cur->equal != NULL) {
            Stack_push(s, cur->equal);
        }
        if (cur->high != NULL) {
            Stack_push(s, cur->high);
        }
        next_cur = cur->low;
        free(cur);
        cur = next_cur;
    };
    free(t);

error:
    if (s != NULL)
        Stack_destroy(s);
    return;
}

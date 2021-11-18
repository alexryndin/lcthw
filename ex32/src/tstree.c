#include "dbg.h"
#include <stack.h>
#include <stdbool.h>
#include <tstree.h>

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

static TSTreeNode *get_node(TSTree *t, const char *key, size_t len,
                            char create, char partial);

void *TSTree_search(TSTree *root, const char *key, size_t len) {
    TSTreeNode *ret = NULL;
    ret = get_node(root, key, len, false, false);

    if (ret != NULL) {
        return ret->value;
    }
    return NULL;
}

void *TSTree_search_prefix(TSTree *root, const char *key, size_t len) {
    TSTreeNode *ret = NULL;
    ret = get_node(root, key, len, false, true);

    if (ret != NULL) {
        while (ret->value == NULL && ret->equal != NULL) {
            ret = ret->equal;
        }
        return ret->value;
    }
    return NULL;
}

static TSTreeNode *get_node(TSTree *t, const char *key, size_t len,
                            char create, char partial) {
    TSTreeNode *cur = NULL;
    CHECK(t != NULL, "null TSTree.");
    CHECK(key != NULL, "null key.");
    CHECK(len > 0, "zero len.");

    if (t->root == NULL) {
        if (create) {
            t->root = calloc(1, sizeof(TSTreeNode));
            t->root->splitchar = key[0];
        } else {
            return NULL;
        }
    }

    cur = t->root;

    while (len > 0) {
        if (key[0] == cur->splitchar) {
            if (len == 1) {
                break;
            }
            if (cur->equal == NULL) {
                if (create) {
                    cur->equal = calloc(1, sizeof(TSTreeNode));
                    CHECK_MEM(cur->equal);
                    cur->equal->splitchar = key[1];
                } else {
                    if (partial) {
                        break;
                    }
                    cur = NULL;
                    break;
                }
            }
            cur = cur->equal;
            len--;
            key++;
            continue;
        } else if (key[0] < cur->splitchar) {
            if (cur->low == NULL) {
                if (create) {
                    cur->low = calloc(1, sizeof(TSTreeNode));
                    CHECK_MEM(cur->equal);
                    cur->low->splitchar = key[0];
                } else {
                    if (partial) {
                        break;
                    }
                    cur = NULL;
                    break;
                }
            }
            cur = cur->low;
            continue;
        } else {
            // key[0] > cur->splitchar
            if (cur->high == NULL) {
                if (create) {
                    cur->high = calloc(1, sizeof(TSTreeNode));
                    CHECK_MEM(cur->equal);
                    cur->high->splitchar = key[0];
                } else {
                    if (partial) {
                        break;
                    }
                    cur = NULL;
                    break;
                }
            }
            cur = cur->high;
            continue;
        }
    }
    // fallthrough

error:
    return cur;
}

// static TSTreeNode *get_node(TSTree *t, const char *key, size_t len,
//                            char create) {
//    TSTreeNode *cur = NULL;
//    CHECK(t != NULL, "null TSTree.");
//    CHECK(key != NULL, "null key.");
//    CHECK(len > 0, "zero len.");
//
//    if (t->root == NULL) {
//        if (create) {
//            t->root = calloc(1, sizeof(TSTreeNode));
//            t->root->splitchar = key[0];
//        } else {
//            return NULL;
//        }
//    }
//
//    cur = t->root;
//
//    while (len > 1) {
//        if (key[0] == cur->splitchar) {
//            if (cur->equal == NULL) {
//                if (create) {
//                    cur->equal = calloc(1, sizeof(TSTreeNode));
//                    cur->equal->splitchar = key[1];
//                } else {
//                    return NULL;
//                }
//
//                cur = cur->equal;
//                len--;
//                key++;
//                continue;
//            }
//        } else if (key[0] < cur->splitchar) {
//            if (cur->low == NULL) {
//                if (create) {
//                    cur->low = calloc(1, sizeof(TSTreeNode));
//                    cur->low->splitchar = key[1];
//                } else {
//                    return NULL;
//                }
//
//                cur = cur->low;
//                continue;
//            }
//        } else {
//            // key[0] > cur->splitchar
//            if (cur->high == NULL) {
//                if (create) {
//                    cur->high = calloc(1, sizeof(TSTreeNode));
//                    cur->high->splitchar = key[1];
//                } else {
//                    return NULL;
//                }
//
//                cur = cur->high;
//                continue;
//            }
//        }
//    }
//
//    // last symbol special case
//    // loop until we find (or not find) node
//    while (1) {
//        if (key[0] == cur->splitchar) {
//            return cur;
//        } else if (key[0] < cur->splitchar) {
//            if (cur->low == NULL) {
//                if (create) {
//                    cur->low = calloc(1, sizeof(TSTreeNode));
//                    cur->low->splitchar = key[0];
//                    return cur->low;
//                } else {
//                    return NULL;
//                }
//                cur = cur->low;
//                continue;
//            } else {
//                // key[0] > cur->splitchar
//                if (cur->high == NULL) {
//                    if (create) {
//                        cur->high = calloc(1, sizeof(TSTreeNode));
//                        cur->high->splitchar = key[0];
//                        return cur->high;
//                    } else {
//                        return NULL;
//                    }
//                    cur = cur->high;
//                    continue;
//                }
//            }
//        }
//    }
// error:
//    return NULL;
//}

TSTreeNode *TSTree_insert(TSTree *t, const char *key, size_t len, void *value) {
    TSTreeNode *cur = NULL;
    CHECK(t != NULL, "null TSTree.");
    CHECK(key != NULL, "null key.");
    CHECK(len > 0, "zero len.");

    cur = get_node(t, key, len, true, false);

    CHECK(cur != NULL, "error getting node.");
    CHECK(cur->value == NULL, "already exists.");

    cur->value = value;

    return cur;

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

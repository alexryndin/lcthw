#include "dbg.h"
#include <bstree.h>
#include <bstrlib/bstrlib.h>
#include <hashmap_utils.h>
#include <stdlib.h>

// int str_cmp(bstring b1, bstring b2) { return bstrcmp(b1, b2); }

BSTree *BSTree_create(BSTree_compare compare) {
    BSTree *t = calloc(1, sizeof(BSTree));
    BSTreeNode *r = calloc(1, sizeof(BSTreeNode));

    CHECK_MEM(t);
    CHECK_MEM(r);

    t->root = r;
    t->compare = compare != NULL ? compare : (BSTree_compare)str_cmp;

    return t;

error:
    if (t != NULL)
        free(t);
    if (r != NULL)
        free(r);
    return NULL;
}

void _BSTree_destroy(BSTreeNode *n, char keys, char values) {
    CHECK(n != NULL, "Null node.");
    if (n->left != NULL)
        _BSTree_destroy(n->left, keys, values);
    if (n->right != NULL)
        _BSTree_destroy(n->right, keys, values);
    if (keys && n->key)
        free(n->key);
    if (values && n->data)
        free(n->data);
    free(n);
error:
    return;
}
void BSTree_destroy(BSTree *t, char keys, char values) {
    CHECK(t != NULL, "Null tree.");
    _BSTree_destroy(t->root, keys, values);

error:
    return;
}

BSTreeNode *BSTree_get_node(BSTreeNode *n, void *key, char create,
                            BSTree_compare cmp) {
    LOG_DEBUG("key is %s", bdata((bstring)key));
    BSTreeNode *ret = NULL;
    CHECK(n != NULL, "Null node.");
    if (n->key == NULL) {
        // possibly root node in empty tree
        CHECK(n->left == NULL && n->right == NULL, "Null key.");
        n->key = key;
        return n;
    }

    ret = n;
    int compare_res = cmp(ret->key, key);
    LOG_DEBUG("comparing %s and %s, result is %d", bdata((bstring)key),
              bdata((bstring)ret->key), compare_res);

    while (1) {
        LOG_DEBUG("comparing %s and %s", bdata((bstring)key),
                  bdata((bstring)ret->key));
        compare_res = cmp(ret->key, key);
        switch (compare_res) {
        case 0:
            LOG_DEBUG("I'm here");
            LOG_DEBUG("data is %s", bdata((bstring)ret->data));
            return ret;
        case -1:
            if (ret->left != NULL) {
                ret = ret->left;
                continue;
            } else {
                if (create) {
                    n = ret;
                    ret = calloc(1, sizeof(BSTreeNode));
                    CHECK_MEM(ret);
                    n->left = ret;
                    ret->parent = n;
                    ret->key = key;
                    return ret;
                } else {
                    return NULL;
                }
            }
        case 1:
            if (ret->right != NULL) {
                ret = ret->right;
                continue;
            } else {
                if (create) {
                    n = ret;
                    ret = calloc(1, sizeof(BSTreeNode));
                    CHECK_MEM(ret);
                    n->right = ret;
                    ret->parent = n;
                    ret->key = key;
                    return ret;
                } else {
                    return NULL;
                }
            }
        default:
            SENTINEL("Wrong compare value.");
        }
    }

error:
    return NULL;
}

int BSTree_set(BSTree *t, void *key, void *data) {
    CHECK(key != NULL, "Null key.");
    CHECK(t != NULL, "Null tree.");
    BSTreeNode *n = BSTree_get_node(t->root, key, 1, t->compare);
    CHECK(n != NULL, "Could't get/create node.");
    n->data = data;
    return 0;

error:
    return -1;
}

void *BSTree_get(BSTree *t, void *key) {
    CHECK(key != NULL, "Null key.");
    CHECK(t != NULL, "Null tree.");
    BSTreeNode *ret = BSTree_get_node(t->root, key, 0, t->compare);
    if (ret == NULL) {
        return NULL;
    } else {
        return ret->data;
    }
error:
    return NULL;
}

BSTreeNode *BSTree_get_min(BSTreeNode *n) {
    CHECK(n != NULL, "Null node.");
    BSTreeNode *ret = n;
    while (1) {
        if (ret->left != NULL) {
            ret = ret->left;
            continue;

        } else {
            return ret;
        }
    }

error:
    return NULL;
}

int _BSTree_traverse(BSTreeNode *n, BSTree_traverse_cb cb) {
    int ret = 0;
    if (n->left != NULL) {
        ret = _BSTree_traverse(n->left, cb);
        if (ret != 0)
            return -1;
    };
    ret = cb(n);
    if (ret != 0)
        return -1;
    if (n->right != NULL) {
        ret = _BSTree_traverse(n->right, cb);
        if (ret != 0)
            return -1;
    };
    return 0;
}

int BSTree_traverse(BSTree *t, BSTree_traverse_cb cb) {
    CHECK(t != NULL, "Null tree.");
    CHECK(t->root != NULL, "Null root.");
    return _BSTree_traverse(t->root, cb);
error:
    return -1;
}

int change_parent(BSTreeNode *cur, BSTreeNode *change) {
    CHECK(cur != NULL, "Null current node.");
    CHECK(cur->parent != NULL, "Null parent node.");

    if (change != NULL)
        change->parent = cur->parent;
    if (cur == cur->parent->left) {
        cur->parent->left = change;
        return 0;
    } else if (cur == cur->parent->right) {
        cur->parent->right = change;
        return 0;
    }
    SENTINEL("Dangling node.");
error:
    return -1;
}

void *BSTree_delete(BSTree *t, void *key, char free_key) {
    CHECK(t != NULL, "Null tree.");
    CHECK(t->root != NULL, "Null root.");
    BSTreeNode *d = BSTree_get_node(t->root, key, 0, t->compare);
    BSTreeNode *ret = NULL;
    BSTreeNode *change = NULL;
    if (d == NULL)
        return d;

    ret = d->data;
    if (d->left == NULL && d->right == NULL) {
        if (free_key) {
            free(d->key);
        }
        d->key = NULL;
        d->data = NULL;
        if (d->parent != NULL) {
            CHECK(change_parent(d, NULL) == 0, "Could't change parent link.");
            free(d);
        }
        return ret;
    }

    if (d->right != NULL && d->left == NULL) {
        if (d->parent != NULL) {
            CHECK(change_parent(d, d->right) == 0,
                  "Could't change parent link.");
        } else {
            t->root = d->right;
            d->right->parent = NULL;
        }
        if (free_key) {
            free(d->key);
            d->key = NULL;
        }
        free(d);
        return ret;
    }

    if (d->left != NULL && d->right == NULL) {
        if (d->parent != NULL) {
            CHECK(change_parent(d, d->left) == 0,
                  "Could't change parent link.");
        } else {
            t->root = d->left;
            d->left->parent = NULL;
        }
        if (free_key) {
            free(d->key);
            d->key = NULL;
        }
        free(d);
        return ret;
    }
    if (d->right->left == NULL) {
        if (free_key) {
            free(d->key);
            d->key = NULL;
        }

        d->key = d->right->key;
        d->data = d->right->data;
        d = d->right;
        CHECK(change_parent(d, d->right) == 0, "Could't change parent link.");
        free(d);
        return ret;
    }

    change = BSTree_get_min(d->right);
    CHECK(change != NULL, "Null min node.");
    if (free_key)
        free(d->key);

    d->key = change->key;
    d->data = change->data;

    CHECK(change_parent(change, change->right) == 0,
          "Could't change parent link.");
    free(change);
    return ret;
error:
    return NULL;
}

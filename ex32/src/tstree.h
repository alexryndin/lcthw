#ifndef tstree_h
#define tstree_h

#include <stdlib.h>

typedef struct TSTreeNode TSTreeNode;

typedef struct TSTree {
    struct TSTreeNode *root;
} TSTree;

void *TSTree_search(TSTree *root, const char *key, size_t len);

void *TSTree_search_prefix(TSTree *root, const char *key, size_t len);

typedef void (*TSTree_traverse_cb)(void *value, void *data);

TSTreeNode *TSTree_insert(TSTree *node, const char *key, size_t len, void *value);

void TSTree_traverse(TSTree *t, TSTree_traverse_cb cb, void *data);

void TSTree_destroy(TSTree *t);

TSTree *TSTree_create();

#endif

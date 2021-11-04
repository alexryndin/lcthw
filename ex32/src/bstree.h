#ifndef bstree_h
#define bstree_h

typedef int (*BSTree_compare)(void *a, void *b);

typedef struct BSTreeNode {
    void *key;
    void *data;

    struct BSTreeNode *left;
    struct BSTreeNode *right;
    struct BSTreeNode *parent;
} BSTreeNode;

typedef struct BSTree {
    int count;
    BSTree_compare compare;
    BSTreeNode *root;
} BSTree;

typedef int (*BSTree_traverse_cb)(BSTreeNode *node);

BSTree *BSTree_create(BSTree_compare compare);
void BSTree_destroy(BSTree *t, char keys, char values);

int BSTree_set(BSTree *t, void *key, void *data);
void *BSTree_get(BSTree *t, void *key);

int BSTree_traverse(BSTree *t, BSTree_traverse_cb cb);

void *BSTree_delete(BSTree *t, void *key, char free_key);

#endif

#include "list_algorithms.h"
#include "../src/dbg.h"
#include "list.h"

int List_bubble_sort(List *l, List_compare cmp) {
    if (List_len(l) < 2)
        return 0;
    char f = 0;
    ListNode *a = NULL;
    ListNode *b = NULL;
    for (int i = 1; i < List_len(l); i++) {
        f = 0;
        a = l->first;
        b = a->next;
        for (int j = 0; j < List_len(l) - i; j++) {
            if (cmp(a->value, b->value) > 0) {
                List_swap(a, b);
            }

            a = a->next;
            b = b->next;
            f = 1;
        }
        if (!f)
            break;
    }
    return 0;
}

List *List_merge_sort(List *l, List_compare cmp) {
    List *ret = NULL;
    CHECK(l != NULL, "Invalid list.");
    int initial_len = List_len(l);
    if (initial_len <= 1) {
        return l;
    }
    List *left = List_create(), *right = NULL;

    List_push_node(left, List_head(l));
    int i = 1;
    while (i < initial_len) {
        if (i < initial_len / 2) {
            List_push_node(left, List_head(l));
        } else {
            right = l;
            break;
        }
        i++;
    }

    left = List_merge_sort(left, cmp);
    right = List_merge_sort(right, cmp);

    ret = List_create();

    while (List_len(left) > 0 && List_len(right) > 0) {
        if (cmp(List_first(left), List_first(right)) < 0)
            List_push_node(ret, List_head(left));
        else
            List_push_node(ret, List_head(right));
    }

    while (List_len(left) > 0)
        List_push_node(ret, List_head(left));

    while (List_len(right) > 0)
        List_push_node(ret, List_head(right));

error:
    if (left != NULL) {
        List_destroy(left);
        left = NULL;
    }
    if (right != NULL) {
        List_destroy(right);
        right = NULL;
    }
    return ret;
}

#ifndef _list_algorithms_h_
#define _list_algorithms_h_

#include "list.h"

typedef int (*List_compare)(void const * const a, void const * const b);

int List_bubble_sort(List *l, List_compare cmp);

List *List_merge_sort(List *l, List_compare cmp);

#endif

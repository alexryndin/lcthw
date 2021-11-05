#ifndef stack_h
#define stack_h

#include <farray.h>

typedef FArray Stack;
#define Stack_create()           FArray_create(sizeof(void *), 0, 10)
#define Stack_destroy(stack)     FArray_destroy(stack)
#define Stack_push(stack, value) FArray_push_pointer(stack, value)
#define Stack_peek(stack)        FArray_last(stack)
#define Stack_count(stack)       FArray_len(stack)
#define Stack_pop(stack)         FArray_pop_pointer(stack)

#define STACK_FOREACH(stack, cur)                           \
    void *cur = NULL;                                       \
    int __i = 0;                                            \
    for (__i = FArray_len(stack), cur = FArray_last(stack); \
         __i > 0 && (cur = FArray_get_pointer(stack, __i - 1), 1); __i--)

#endif

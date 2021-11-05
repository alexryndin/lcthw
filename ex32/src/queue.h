#ifndef queue_h
#define queue_h

#include <list.h>

typedef List Queue;
#define Queue_create()           List_create()
#define Queue_destroy(queue)     List_destroy(queue)
#define Queue_send(queue, value) List_push(queue, value)
#define Queue_peek(queue)        List_last(queue)
#define Queue_count(queue)       List_len(queue)
#define Queue_recv(queue)        List_pop(queue)

#define QUEUE_FOREACH(queue, cur) LIST_FOREACH(queue, last, prev, cur)

#endif

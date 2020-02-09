#include <kernel/defines.h>
#include <lib/clist.h>
#include <types.h>

#ifndef LIB_QUEUE_H_
#define LIB_QUEUE_H_

/*
	just queue
*/
/*
struct queue_t {
    void *base;
    void *front, *rear;
    size_t size, capacity;  // size of one element, capacity of entire queue
};
*/
/*
extern struct queue_t *queue_create(size_t size, size_t capacity);
extern void queue_push(struct queue_t *q, void *item);
extern void *queue_pop(struct queue_t *q);
*/

struct queue_head_t {
    struct clist_head_t head;
    void *data;
};

struct queue_def_t {
    struct clist_def_t clist_head;
};

extern struct queue_head_t *queue_init(struct queue_def_t *ct);
extern struct queue_head_t *queue_push(struct queue_def_t *ct);
extern struct queue_head_t *queue_pop(struct queue_def_t *ct, struct queue_head_t *buff);

#endif

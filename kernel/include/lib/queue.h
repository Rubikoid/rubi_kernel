#include <kernel/defines.h>
#include <types.h>

#ifndef LIB_QUEUE_H_
#define LIB_QUEUE_H_

/*
	just queue
*/

struct queue_t {
    void *base;
    size_t front, rear, size, capacity;
};

extern struct queue_t *queue_create(size_t size);
extern void queue_push(struct queue_t *q, uint8_t item);
extern uint8_t queue_pop(struct queue_t *q);

#endif

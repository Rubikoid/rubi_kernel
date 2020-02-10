#include <lib/queue.h>
#include <lib/string.h>

/*
struct queue_t *queue_create(size_t size, size_t capacity) {

    struct queue_t *q = (struct queue_t *)kmalloc(sizeof(struct queue_t));
    q->size = size;
    q->capacity = capacity;
    q->base = kmalloc(q->size * q->capacity);
    q->front = 0;
    q->rear = 0;

    return q;
}

void queue_push(struct queue_t *q, uint8_t item) {
    //((uint8_t *)q->base)[q->end--] = item;
    //    q->end %= q->n;
}

uint8_t queue_pop(struct queue_t *q) {
    //  if (q->last < 1)
    //      return 0;
    //  return ((uint8_t *)q->base)[--q->last];
    return 0;
}
*/

struct queue_head_t *queue_init(struct queue_def_t *ct) {
    struct clist_head_t *entry = clist_init(&ct->clist_head);
    struct queue_head_t *qe = (struct queue_head_t *)entry->data;
    qe->data = qe;
    return qe;
}

struct queue_head_t *queue_push(struct queue_def_t *ct) {
    struct clist_head_t *entry = clist_insert_after(&ct->clist_head, ct->clist_head.tail);  // insert to tail;
    struct queue_head_t *qe = (struct queue_head_t *)entry->data;
    qe->data = qe;
    return qe;
}

struct queue_head_t *queue_pop(struct queue_def_t *ct, struct queue_head_t *buff) {
    struct clist_head_t *entry = ct->clist_head.head;  // insert to tail;
    if (entry != NULL) {
        memcpy(buff, entry, ct->clist_head.slot_size);
        clist_delete(&ct->clist_head, entry);
    } else
        return NULL;
    return buff;
}

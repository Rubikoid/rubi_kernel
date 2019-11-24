#include <lib/queue.h>

struct queue_t *queue_create(size_t size) {
    return 0;
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

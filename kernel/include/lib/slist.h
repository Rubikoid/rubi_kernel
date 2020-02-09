#include <kernel/defines.h>
#include <types.h>

#ifndef LIB_SLIST_H_
#define LIB_SLIST_H_

/*
	static list, two-direction linked.
*/

struct slist_head_t {
    struct slist_head_t *prev;
    struct slist_head_t *next;
    uint8_t isValid;
    void *data;
};

struct slist_def_t {
    size_t base;
    uint32_t slots;
    size_t slot_size;
    struct slist_head_t *head;
    struct slist_head_t *tail;
};

extern struct slist_head_t *slist_insert_after(struct slist_def_t *list, struct slist_head_t *pos);
extern struct slist_head_t *slist_insert_before(struct slist_def_t *list, struct slist_head_t *pos);
extern void slist_delete(struct slist_def_t *list, struct slist_head_t *pos);

extern void slist_dump(struct slist_def_t *list);

extern void slist_test();
#endif

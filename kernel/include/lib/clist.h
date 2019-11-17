#include <kernel/defines.h>
#include <types.h>

#ifndef LIB_CLIST_H_
#define LIB_CLIST_H_

struct clist_head_t {
    struct clist_head_t *next;
    void *data;
};

struct clist_def_t {
    uint32_t slots;
    size_t slot_size;
    struct clist_head_t *head;
};

extern struct clist_head_t *clist_init(struct clist_def_t *ct);
extern struct clist_head_t *clist_insert_after(struct clist_def_t *ct, struct clist_head_t *pos);
extern void clist_delete(struct clist_def_t *ct, struct clist_head_t *pos);

extern void clist_test();
#endif

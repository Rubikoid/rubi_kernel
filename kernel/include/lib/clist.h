#include <kernel/defines.h>
#include <types.h>

#ifndef LIB_CLIST_H_
#define LIB_CLIST_H_

/*
	cyclic list.
	must be dynamic, kernel implementation uses kmalloc
	user-mode implementation also should use dynamic memory
*/

struct clist_head_t {
    struct clist_head_t *next;
    void *data;
};

struct clist_def_t {
    uint32_t slots;
    size_t slot_size;
    struct clist_head_t *head;
};

typedef uint8_t (*clist_find_fn_t)(struct clist_head_t *entry, va_list list);


extern struct clist_head_t *clist_init(struct clist_def_t *ct);
extern struct clist_head_t *clist_insert_after(struct clist_def_t *ct, struct clist_head_t *pos);
extern struct clist_head_t *clist_find(struct clist_def_t *def, clist_find_fn_t func, ...);
extern void clist_delete(struct clist_def_t *ct, struct clist_head_t *pos);

extern void clist_test();
#endif

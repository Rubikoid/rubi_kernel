#include <lib/clist.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>

struct clist_head_t *clist_init(struct clist_def_t *ct) {
    struct clist_head_t *n = syscall_malloc(ct->slot_size);
    memset(n, 0, ct->slot_size);
    n->next = n;
    n->data = n;
    ct->head = n;
    ct->tail = n;
    ct->slots = 1;
    return n;
}

struct clist_head_t *clist_insert_after(struct clist_def_t *ct, struct clist_head_t *pos) {
    if (pos == NULL || ct->slots == 0)
        return clist_init(ct);
    assert(ct->slots != 0);
    struct clist_head_t *n = syscall_malloc(ct->slot_size);
    n->next = pos->next;
    n->data = n;
    pos->next = n;
    if (ct->tail == pos)
        ct->tail = n;
    ct->slots += 1;

    // ct->head = n; // а это вот зачем блин
    return n;
}

void clist_delete(struct clist_def_t *ct, struct clist_head_t *pos) {
    if (ct->slots == 1) {
        ct->head = NULL;
        ct->tail = NULL;
    } else {
        struct clist_head_t *x = ct->head;
        for (int i = 0; i <= ct->slots; i++) {  // find previous list item
            //printf("%x->%x == %x\n", x, x->next, pos);
            if (x->next == pos)
                break;
            x = x->next;
        }
        if (pos == ct->head)
            ct->head = x;
        if (pos == ct->tail)
            ct->tail = x;
        x->next = pos->next;
    }
    ct->slots -= 1;
    syscall_free(pos);
}

struct clist_head_t *clist_get(struct clist_def_t *def, uint32_t index) {
    if (index >= def->slots)
        return NULL;
    struct clist_head_t *start = def->head;  // idk, i think where are no way to do it on O(1), or we need smth like other table with indexes, maybe.
    for (int i = 0; i < index; i++)
        start = start->next;
    return start;
}

struct clist_head_t *clist_find(struct clist_def_t *ct, clist_find_fn_t func, ...) {
    struct clist_head_t *cur = ct->head;
    if (cur == NULL)
        return NULL;
    //for (uint32_t i = 0; i <= ct->slots; i++) {
    do {
        va_list list;
        va_start(list, func);
        if (func(cur, list))
            return cur;
        cur = cur->next;
    } while (cur != ct->head && cur != NULL);
    return NULL;
}

void clist_test() {
    struct clist_def_t ct = {
        .slots = 0,
        .slot_size = sizeof(struct clist_head_t),
        .head = 0,
    };
    struct clist_head_t *a1 = clist_init(&ct);
    assert(ct.slots == 1);
    assert(a1 == ct.head);
    assert(a1->next == a1);

    struct clist_head_t *a2 = clist_insert_after(&ct, a1);
    assert(a1->next == a2);
    assert(a2->next == a1);
    assert(ct.head == a1);  // это тоже вот зачем было a2

    struct clist_head_t *a3 = clist_insert_after(&ct, a2);
    assert(a1->next == a2);
    assert(a2->next == a3);
    assert(a3->next == a1);
    assert(ct.head == a1);  // это тоже вот зачем было a3

    // a1 -> a2 -> a4 -> a3 -> a1
    struct clist_head_t *a4 = clist_insert_after(&ct, a2);
    assert(a1->next == a2);
    assert(a2->next == a4);
    assert(a4->next == a3);
    assert(a3->next == a1);
    assert(ct.head == a1);  // это тоже вот зачем было a4
    assert(ct.slots == 4);

    // a1 -> a4 -> a3 -> a1
    clist_delete(&ct, a2);
    assert(a1->next == a4);
    assert(a4->next == a3);
    assert(a3->next == a1);
    assert(ct.head == a1);  // это тоже вот зачем было a4

    // a1 -> a3 -> a1
    clist_delete(&ct, a4);
    assert(a1->next == a3);
    assert(a3->next == a1);
    assert(ct.head == a1);

    // a3
    clist_delete(&ct, a1);
    assert(ct.head == a3);
    assert(ct.slots == 1);
    assert(ct.head == a3);

    // nothing
    clist_delete(&ct, a3);
    assert(ct.slots == 0);
}

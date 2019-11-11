#include <lib/slist.h>
#include <lib/stdio.h>
#include <lib/string.h>

/*
    1.
        _	-> x
        pos = 0
            ->
                x.prev = 0
                x.next = 0
                list.head = x
                list.tail = x
        
    2.
        a, b -> a, b, x
        pos = b
            ->
                b.next = x
                x.prev = b
                x.next = 0
                list.tail = x

    3.
        a, b -> a, x, b
        pos = a
            ->
                a.next = x
                x.prev = a
                x.next = b
                b.prev = x

    4.
        a, b -> x, a, b
        pos = 0
            ->
                x.prev = 0
                x.next = a
                a.prev = x
                list.head = x
*/
struct slist_head_t *slist_insert_after(struct slist_def_t *list, struct slist_head_t *pos) {
    if (!list) return NULL;
    struct slist_head_t *elem = NULL, *prev = NULL, *next = NULL;
    size_t offset = 0;
    for (int i = 0; i < list->slots; i++) {
        offset = list->base + i * list->slot_size;
        elem = (struct slist_head_t *)offset;
        if (!elem->isValid) break;
        if (i == list->slots - 1) return NULL;
    }
    memset((void *)offset, 0, list->slot_size);
    elem->isValid = 1;
    elem->data = (void *)offset;

    if (pos == NULL) {
        prev = NULL;
        next = list->head;
    } else {
        prev = pos;
        next = pos->next;
    }

    elem->prev = prev;
    elem->next = next;

    if (prev != NULL)
        prev->next = elem;
    if (next != NULL)
        next->prev = elem;

    if (pos == list->tail)
        list->tail = elem;
    if (list->head == NULL || pos == NULL)
        list->head = elem;
    return elem;
}

/*
    1.
        _	-> x
        pos = 0
            ->
                x.prev = 0
                x.next = 0
                list.head = x
                list.tail = x
        
    2.
        a -> a, x
        pos = 0
            ->
                a.next = x
                x.prev = a
                x.next = 0
                list.tail = x

    3.
        a, b -> a, x, b
        pos = b
            ->
                a.next = x
                x.prev = a
                x.next = b
                b.prev = x

    4.
        a, b -> x, a, b
        pos = a
            ->
                x.prev = 0
                x.next = a
                a.prev = x
                list.head = x
*/
struct slist_head_t *slist_insert_before(struct slist_def_t *list, struct slist_head_t *pos) {
    if (!list) return NULL;
    struct slist_head_t *elem = NULL, *prev = NULL, *next = NULL;
    size_t offset = 0;
    for (int i = 0; i < list->slots; i++) {
        offset = list->base + i * list->slot_size;
        elem = (struct slist_head_t *)offset;
        if (!elem->isValid) break;
        if (i == list->slots - 1) return NULL;
    }
    memset((void *)offset, 0, list->slot_size);
    elem->isValid = 1;
    elem->data = (void *)offset;

    if (pos == NULL) {
        next = NULL;
        prev = list->tail;
    } else {
        next = pos;
        prev = pos->prev;
    }

    elem->prev = prev;
    elem->next = next;

    if (prev != NULL)
        prev->next = elem;
    if (next != NULL)
        next->prev = elem;

    if (list->head == pos)
        list->head = elem;
    if (list->tail == NULL || pos == NULL)
        list->tail = elem;
    return elem;
}

/*
    1.
        x	-> 
                list.head = 0
                list.tail = 0
    2.
        a, x -> a
            ->
                a.next = 0
                list.tail = a

    3.
        a, x, b -> a, b
            ->
                a.next = b
                b.prev = a

    4.
        x, a -> a
                a.prev = 0
                list.head = a
*/
void slist_delete(struct slist_def_t *list, struct slist_head_t *pos) {
    if (!pos || !list) return;
    struct slist_head_t *prev = pos->prev;
    struct slist_head_t *next = pos->next;
    pos->isValid = 0;

    if (prev != NULL)
        prev->next = next;
    if (next != NULL)
        next->prev = prev;

    if (pos == list->head)
        list->head = next;
    if (pos == list->tail)
        list->tail = prev;

    memset(pos->data, 0, list->slot_size);
}

void slist_dump(struct slist_def_t *list) {
    printf("-- slist dump\n");
    for(struct slist_head_t *elem = list->head; elem != NULL; elem = elem->next) {
        printf("    t=%x p=%x n=%x\n", elem, elem->prev, elem->next);
    }
}

void slist_test() {
    struct slist_head_t data[10];
    struct slist_def_t list = {
        .base = (size_t)data,
        .slots = 10,
        .slot_size = sizeof(struct slist_head_t),
        .head = NULL,
        .tail = NULL,
    };
    memset(data, 0, sizeof(data));
    {
        // insert after
        // first case
        struct slist_head_t *fir = slist_insert_after(&list, NULL);
        assert(fir != NULL);
        assert(fir->prev == NULL);
        assert(fir->next == NULL);
        assert(list.head == fir);
        assert(list.tail == fir);

        // second case
        struct slist_head_t *sec = slist_insert_after(&list, fir);
        assert(sec != NULL);
        assert(fir->next == sec);
        assert(sec->prev == fir);
        assert(sec->next == NULL);
        assert(list.tail == sec);

        // third case
        struct slist_head_t *third = slist_insert_after(&list, fir);
        assert(third != NULL);
        assert(fir->next == third);
        assert(third->prev == fir);
        assert(third->next == sec);
        assert(sec->prev == third);

        // fourth case
        struct slist_head_t *fourth = slist_insert_after(&list, NULL);
        assert(fourth != NULL);
        assert(fourth->prev == NULL);
        assert(fourth->next == fir);
        assert(fir->prev == fourth);
        assert(list.head == fourth);

        // in result: fourth, fir, third, sec

        // delete
        // third case
        slist_delete(&list, third);
        assert(fir->next == sec);
        assert(sec->prev == fir);

        // second case
        slist_delete(&list, sec);
        assert(fir->next == NULL);
        assert(list.tail == fir);

        // fourth case
        slist_delete(&list, fourth);
        assert(fir->prev == NULL);
        assert(list.head == fir);

        // first case
        slist_delete(&list, fir);
        assert(list.head == NULL);
        assert(list.tail == NULL);
    }
    {
        // insert before
        // first case
        struct slist_head_t *fir = slist_insert_before(&list, NULL);
        assert(fir != NULL);
        assert(fir->prev == NULL);
        assert(fir->next == NULL);
        assert(list.head == fir);
        assert(list.tail == fir);

        // second case
        struct slist_head_t *sec = slist_insert_before(&list, NULL);
        assert(sec != NULL);
        assert(fir->next == sec);
        assert(sec->prev == fir);
        assert(sec->next == NULL);
        assert(list.tail == sec);

        // third case
        struct slist_head_t *third = slist_insert_before(&list, sec);
        assert(third != NULL);
        assert(fir->next == third);
        assert(third->prev == fir);
        assert(third->next == sec);
        assert(sec->prev == third);

        // fourth case
        struct slist_head_t *fourth = slist_insert_before(&list, fir);
        assert(fourth != NULL);
        assert(fourth->prev == NULL);
        assert(fourth->next == fir);
        assert(fir->prev == fourth);
        assert(list.head == fourth);
    }
    // slist_dump(&list);
}

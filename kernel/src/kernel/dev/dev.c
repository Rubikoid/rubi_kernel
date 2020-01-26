#include <kernel/dev/dev.h>
#include <lib/clist.h>

struct clist_def_t dev_list = {
    .slots = 0,
    .slot_size = sizeof(struct dev_t),
    .head = 0,
};

struct dev_t* dev_create() {
    struct clist_head_t* entry;
    struct dev_t* device;

    entry = clist_insert_after(&dev_list, dev_list.head);
    device = (struct dev_t*)entry->data;

    return device;
}

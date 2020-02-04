#include <kernel/dev/dev.h>
#include <lib/clist.h>
#include <lib/string.h>

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


uint8_t dev_by_name_finder(struct clist_head_t *entry, va_list list) {
    struct dev_t *dev = (struct dev_t *) entry;
    uint8_t *name = va_arg(list, uint8_t *);
    return !strcmp((char *)name, dev->name);
}

struct dev_t *find_dev_by_name(char *name) {
    struct clist_head_t* entry;
    entry = clist_find(&dev_list, dev_by_name_finder, name);
    return (struct dev_t*)entry->data;
}

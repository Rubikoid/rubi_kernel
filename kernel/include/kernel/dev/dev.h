#include <lib/clist.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_DEV_DEV_H_
#define KERNEL_DEV_DEV_H_

typedef uint32_t (*dev_read_fn_t)(FILE *io_buf, void *buf, uint32_t size);
typedef void (*dev_write_fn_t)(FILE *io_buf, void *buf, uint32_t size);
typedef void (*dev_cmd_fn_t)(FILE *io_buf, uint32_t command);

struct dev_t {
    struct clist_head_t list_head;
    char name[8];
    void *base_r;
    void *base_w;
    dev_read_fn_t read_fn;
    dev_write_fn_t write_fn;
    dev_cmd_fn_t cmd_fn;
    struct clist_def_t ih_list;  // low half interrupt handlers
};

struct ih_low_data_t {
    uint32_t number;
    void *data;
};

typedef void (*ih_low_fn_t)(uint32_t number, struct ih_low_data_t *data);

struct ih_low_t {
    struct clist_head_t list_head;
    uint32_t number;
    ih_low_fn_t handler;
};

extern struct clist_def_t dev_list;

extern struct dev_t *dev_create();
extern struct dev_t *find_dev_by_name(char *name);

#endif

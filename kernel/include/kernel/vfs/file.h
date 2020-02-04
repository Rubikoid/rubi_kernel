#include <kernel/dev/dev.h>
#include <lib/clist.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_VFS_FILE_H_
#define KERNEL_VFS_FILE_H_

struct file_t {
    struct clist_head_t list_head;
    FILE io_buf;
    char name[8];
    uint16_t mod_rw;
    struct dev_t *dev;
};

extern struct clist_def_t file_list;
extern uint32_t next_fd;

extern FILE *file_open(uint8_t *path, uint16_t mode_rw);

#endif

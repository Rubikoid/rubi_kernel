#include <kernel/dev/dev.h>
#include <lib/clist.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_VFS_FILE_H_
#define KERNEL_VFS_FILE_H_

#define FILE_READ 	0
#define FILE_WRITE 	1

// this is a kernel-only struct, used for tracking open file descriptors and their assoc with devices.
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
extern size_t file_read(struct io_buf_t *io_buf, char *buff, uint32_t size);
extern size_t file_write(struct io_buf_t *io_buf, char *buff, uint32_t size);
extern void file_ioctl(struct io_buf_t *io_buf, uint32_t cmd);

#endif

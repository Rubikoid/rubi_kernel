#include <kernel/dev/dev.h>
#include <lib/clist.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_VFS_FILE_H_
#define KERNEL_VFS_FILE_H_

#define FILE_READ  0
#define FILE_WRITE 1

// this is a kernel-only struct, used for tracking open file descriptors.
struct file_t;
struct fs_node_t;

typedef uint32_t (*read_func_t)(struct file_t *file, uint32_t *offset, uint32_t size, uint8_t *buff);
typedef uint32_t (*write_func_t)(struct file_t *file, uint32_t *offset, uint32_t size, uint8_t *buff);

struct file_t {
    struct clist_head_t list_head;
    uint8_t *pos;            // pos in buff
    struct fs_node_t *node;  // pointer to fs node
    uint16_t mode;           // mode?

    void *private_data;  // shrug
    read_func_t read;
    write_func_t write;
    //FILE io_buf;
    //char name[8];
    //struct dev_t *dev;
};

// extern struct clist_def_t file_list;
// extern uint32_t next_fd;

extern uint32_t file_open(uint8_t *path, uint16_t mode);
extern size_t file_read(uint32_t fd, char *buff, uint32_t size);
extern size_t file_write(uint32_t fd, char *buff, uint32_t size);
extern void file_ioctl(uint32_t fd, uint32_t cmd);

#endif

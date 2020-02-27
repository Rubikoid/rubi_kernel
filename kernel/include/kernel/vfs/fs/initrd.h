#include <kernel/vfs/file.h>
#include <kernel/vfs/node.h>
#include <lib/clist.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_VFS_FS_INITRD_H_
#define KERNEL_VFS_FS_INITRD_H_

#define HEADMAGIC 0xABCDABCD
#define FILEMAGIC 0xAABBCCDD

#define INITRDFS_FILE 0x01
#define INITRDFS_DIR  0x02

/*
    initrd fs view:
    
    initrd_head_t
    initrd_file_head_t
    <directory, count=initrd_file_head_t.size>
    initrd_file_head_t
    <file, size=initrd_file_head_t.size>
    initrd_file_head_t
    <file, size=initrd_file_head_t.size>
*/

struct __attribute__((__packed__)) initrd_head_t {
    uint32_t magic;
    uint32_t offset;
};

struct __attribute__((__packed__)) initrd_file_head_t {
    uint32_t magic;
    uint32_t size;       // size or count if this is a directory
    uint32_t name_size;  // size of name. without \0
    uint8_t type;
};

struct initrd_status_t {
    struct initrd_head_t *head;
    struct initrd_file_head_t* root;
};

extern struct fs_node_t *initrd_create_node(uint32_t offset, struct initrd_file_head_t *data);
extern void initrd_init(void *ptr);

extern int initrd_readdir(struct fs_node_t *node, uint32_t num, struct dirent_t *dirent);
extern struct fs_node_t *initrd_openddir(struct fs_node_t *node, uint32_t num);

extern uint32_t initrd_read(struct file_t *file, uint32_t *offset, uint32_t size, uint8_t *buff);
extern uint32_t initrd_write(struct file_t *file, uint32_t *offset, uint32_t size, uint8_t *buff);
extern uint32_t initrd_ioctl(struct file_t *file, uint32_t cmd, uint32_t subcmd);
extern int initrd_open(struct fs_node_t *node, struct file_t *file);

extern void initrd_test(void *ptr); 

#endif

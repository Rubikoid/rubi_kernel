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

extern void initrd_init(void *ptr);
extern void initrd_test(void *ptr); 

#endif

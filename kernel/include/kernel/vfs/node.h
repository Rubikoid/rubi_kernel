#include <kernel/vfs/file.h>
#include <lib/clist.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_VFS_NODE_H_
#define KERNEL_VFS_NODE_H_

#define NODE_NAME_SIZE 128

#define FS_NONE       0x00
#define FS_FILE       0x01
#define FS_DIRECTORY  0x02
#define FS_CHRDEVICE  0x03
#define FS_BLKDEVICE  0x04
#define FS_PIPE       0x05
#define FS_SYMLINK    0x06
#define FS_MOUNTPOINT 0x08  // OR with directory ;F

struct fs_node_t;
struct dirent_t;

typedef void (*fs_open_func_t)(struct fs_node* node, struct file_t* file);
typedef void (*fs_close_func_t)(struct fs_node* node, struct file_t* file);

typedef struct dirent_t* (*readdir_func_t)(struct fs_node* node, uint32_t num);
typedef struct fs_node* (*finddir_func_t)(struct fs_node* node, char* name);

struct fs_node_t {
    struct clist_head_t list_head;  // maybe i need list head?
    char name[NODE_NAME_SIZE];      // name of node
    uint32_t flags;                 // flags, like directory, file, link, etc
    uint32_t length;                // file size
    uint32_t inode;                 // WTF: is thaaat
    uint32_t impl;                  // WTF: x2
    fs_open_func_t open;
    fs_close_func_t close;
    readdir_func_t readdir;
    finddir_func_t finddir;
    struct fs_node_t* ptr;  // link?
};

struct dirent_t {
    char name[NODE_NAME_SIZE];
    uint32_t ino;  // wat
};

#endif

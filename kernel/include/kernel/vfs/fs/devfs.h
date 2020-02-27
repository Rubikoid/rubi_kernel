#include <kernel/vfs/file.h>
#include <kernel/vfs/node.h>
#include <lib/clist.h>
#include <lib/stdio.h>
#include <types.h>

#ifndef KERNEL_VFS_FS_DEVFS_H_
#define KERNEL_VFS_FS_DEVFS_H_

extern struct fs_node_t *devfs_create_node(uint32_t id);
extern struct fs_node_t *devfs_mount(struct fs_node_t *node);

extern int devfs_readdir(struct fs_node_t *node, uint32_t num, struct dirent_t *dirent);
extern struct fs_node_t *devfs_openddir(struct fs_node_t *node, uint32_t num);

extern uint32_t devfs_read(struct file_t *file, uint32_t *offset, uint32_t size, uint8_t *buff);
extern uint32_t devfs_write(struct file_t *file, uint32_t *offset, uint32_t size, uint8_t *buff);
extern uint32_t devfs_ioctl(struct file_t *file, uint32_t cmd, uint32_t subcmd);
extern int devfs_open(struct fs_node_t *node, struct file_t *file);


#endif

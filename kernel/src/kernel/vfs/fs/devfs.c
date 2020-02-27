#include <kernel/dev/dev.h>
#include <kernel/memory/heap.h>
#include <kernel/vfs/fs/devfs.h>
#include <kernel/vfs/node.h>
#include <lib/stdio.h>
#include <lib/string.h>

struct fs_node_t *devfs_create_node(uint32_t id) {
    struct fs_node_t *node = find_fs_node_by_ino(id);
    if (node != NULL)
        return node;
    node = get_new_node();

    struct dev_t *dev;
    {
        struct clist_head_t *entry = clist_get(&dev_list, id);
        if (entry == NULL)
            return NULL;
        dev = (struct dev_t *)entry->data;
    }

    memcpy(node->name, dev->name, sizeof(dev->name));
    node->name[sizeof(dev->name)] = '\0';

    node->flags = 0;
    node->flags |= FS_CHRDEVICE;
    //if (data->type == INITRDFS_FILE) {
    //    node->flags |= FS_FILE;
    //} else if (data->type == INITRDFS_DIR)
    //    node->flags |= FS_DIRECTORY;

    node->length = 0;
    node->inode = id;
    node->impl = 1;

    node->open = devfs_open;
    node->close = 0;
    node->readdir = devfs_readdir;
    node->opennode = devfs_openddir;
    node->finddir = 0;

    node->ptr = 0;

    return node;;
}

struct fs_node_t *devfs_mount(struct fs_node_t *node) {  // TODO: fix that whack
    //struct fs_node_t *node = get_new_node();

    // strcpy(node->name, "dev");

    node->flags = FS_DIRECTORY | FS_MOUNTPOINT;
    node->length = dev_list.slots;
    // node->inode = 0;
    node->impl = 1;

    node->open = devfs_open;
    node->close = 0;
    node->readdir = devfs_readdir;
    node->opennode = devfs_openddir;
    node->finddir = 0;

    node->ptr = 0;

    return node;
}

int devfs_readdir(struct fs_node_t *node, uint32_t num, struct dirent_t *dirent) {
    if((node->flags & FS_DIRECTORY) != FS_DIRECTORY)
        return 0;
    if(num >= dev_list.slots)
        return 0;

    return 1;
}

uint32_t devfs_read(struct file_t *file, uint32_t *offset, uint32_t size, uint8_t *buff) {
    int ret = 0;
    // if (*offset >= file->node->length)
    //     return 0;
    // uint8_t *base_ptr = 0;
    // ret = (*offset + size >= file->node->length) ? file->node->length - *offset : size;
    // printf("Count: %x, off: %x, size: %x, node_size: %x\n", ret, *offset, size, file->node->length);
    // memcpy(buff, base_ptr + *offset, ret);
    return ret;
}

uint32_t devfs_write(struct file_t *file, uint32_t *offset, uint32_t size, uint8_t *buff) {
    return 0;
}

int devfs_open(struct fs_node_t *node, struct file_t *file) {
    file->node = node;
    file->read = devfs_read;
    file->write = devfs_write;
    return 1;
}

struct fs_node_t *devfs_openddir(struct fs_node_t *node, uint32_t num) {
    struct fs_node_t *ret_node = devfs_create_node(num);
    return ret_node;
}

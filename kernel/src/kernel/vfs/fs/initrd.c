#include <kernel/memory/heap.h>
#include <kernel/vfs/fs/initrd.h>
#include <kernel/vfs/node.h>
#include <lib/stdio.h>
#include <lib/string.h>

#define __MODULE_NAME__ "IRD"

struct initrd_status_t ird_stat = {0};

struct fs_node_t *initrd_create_node(uint32_t offset, struct initrd_file_head_t *data) {
    struct fs_node_t *node = find_fs_node_by_ino(offset);
    if (node != NULL)
        return node;
    node = get_new_node();

    memcpy(node->name, (void *)data + sizeof(struct initrd_file_head_t), data->name_size);
    node->name[data->name_size] = '\0';

    node->flags = 0;
    if (data->type == INITRDFS_FILE) {
        node->flags |= FS_FILE;
    } else if (data->type == INITRDFS_DIR)
        node->flags |= FS_DIRECTORY;

    node->length = data->size;
    node->inode = offset;
    node->impl = 0;

    node->open = initrd_open;
    node->close = 0;
    node->readdir = initrd_readdir;
    node->opennode = initrd_openddir;
    node->finddir = 0;

    node->ptr = 0;

    return node;
}

void initrd_init(void *ptr) {
    ird_stat.head = ptr;
    klog("Staring on initrd on %x\n", ptr);
    if (ird_stat.head->magic != HEADMAGIC) {
        klog("Bad head magic\n");
        return;
    }
    klog("offset: %x\n", ird_stat.head->offset);
    ird_stat.root = ptr + ird_stat.head->offset;
    if (ird_stat.root->magic != FILEMAGIC) {
        klog("Bad root magic\n");
        return;
    }
    {
        struct fs_node_t *root_node = find_fs_node_by_ino(ird_stat.head->offset);
        if (root_node != NULL) {
            klog("WTF why root node exists?\n");
            return;
        }
        if (ird_stat.root->type == INITRDFS_FILE) {
            klog("WTF why root node is file??\n");
            return;
        }
        root_node = initrd_create_node(ird_stat.head->offset, ird_stat.root);
    }
    klog("Root node ok\n");
}

int initrd_readdir(struct fs_node_t *node, uint32_t num, struct dirent_t *dirent) {
    struct initrd_file_head_t *entry = (struct initrd_file_head_t *)((uint8_t *)ird_stat.head + node->inode);
    if (entry->magic != FILEMAGIC)
        return 0;
    if (entry->type != INITRDFS_DIR)
        return 0;
    if (num >= entry->size)
        return 0;

    /*
        struct - sizeof(struct)
        name - struct.name_len
        ptr1 - sizeof(size_t)
        ptr2 - sizeof(size_t)
        ptr3 - sizeof(size_t)
    */

    dirent->ino = *((size_t *)((uint8_t *)entry + sizeof(struct initrd_file_head_t) + entry->name_size) + num);
    struct initrd_file_head_t *inner_entry = (struct initrd_file_head_t *)((uint8_t *)ird_stat.head + dirent->ino);
    if (entry->magic != FILEMAGIC) {
        dirent->ino = 0;
        return 0;
    }
    // strncpy(dirent->name, (char *)inner_entry + sizeof(struct initrd_file_head_t), sizeof(dirent->name));
    memcpy(dirent->name, (void *)inner_entry + sizeof(struct initrd_file_head_t), inner_entry->name_size);
    dirent->name[inner_entry->name_size] = '\0';
    return 1;
}

uint32_t initrd_read(struct file_t *file, uint32_t *offset, uint32_t size, uint8_t *buff) {
    struct initrd_file_head_t *entry = (struct initrd_file_head_t *)((uint8_t *)ird_stat.head + file->node->inode);
    int ret = 0;
    if (*offset >= entry->size)
        return 0;
    uint8_t *base_ptr = (uint8_t *)entry + sizeof(struct initrd_file_head_t) + entry->name_size;
    ret = (*offset + size >= entry->size) ? entry->size - *offset : size;
    printf("Count: %x, off: %x, size: %x, fsize: %x, node_size: %x\n", ret, *offset, size, entry->size, file->node->length);
    memcpy(buff, base_ptr + *offset, ret);
    return ret;
}

uint32_t initrd_write(struct file_t *file, uint32_t *offset, uint32_t size, uint8_t *buff) {
    return 0;
}

uint32_t initrd_ioctl(struct file_t *file, uint32_t cmd, uint32_t subcmd) {
    return 0;
}

int initrd_open(struct fs_node_t *node, struct file_t *file) {
    struct initrd_file_head_t *entry = (struct initrd_file_head_t *)((uint8_t *)ird_stat.head + node->inode);
    if (entry->type != INITRDFS_FILE)
        return 0;

    file->node = node;
    file->read = initrd_read;
    file->write = initrd_write;
    file->ioctl = initrd_ioctl;
    return 1;
}

struct fs_node_t *initrd_openddir(struct fs_node_t *node, uint32_t num) {
    struct initrd_file_head_t *entry = (struct initrd_file_head_t *)((uint8_t *)ird_stat.head + node->inode);
    if (entry->magic != FILEMAGIC)
        return 0;
    if (entry->type != INITRDFS_DIR)
        return 0;
    if (num >= entry->size)
        return 0;

    int next_num = *((size_t *)((uint8_t *)entry + sizeof(struct initrd_file_head_t) + entry->name_size) + num);
    struct initrd_file_head_t *inner_entry = (struct initrd_file_head_t *)((uint8_t *)ird_stat.head + next_num);
    if (entry->magic != FILEMAGIC)
        return 0;

    struct fs_node_t *ret_node = initrd_create_node(next_num, inner_entry);
    return ret_node;
}

void initrd_test(void *ptr) {
    printf("STARTING TEST\n");

    struct initrd_head_t *head = ptr;
    if (head->magic != HEADMAGIC) {
        printf("Bad head magic\n");
        return;
    }
    printf("offset: %x\n", head->offset);
    struct initrd_file_head_t *initrd_root = ptr + head->offset;
    if (initrd_root->magic != FILEMAGIC) {
        printf("Bad root magic\n");
        return;
    }
}

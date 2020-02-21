#include <kernel/memory/heap.h>
#include <kernel/vfs/fs/initrd.h>
#include <kernel/vfs/node.h>
#include <lib/stdio.h>
#include <lib/string.h>

struct initrd_status_t ird_stat = {0};

void initrd_init(void *ptr) {
    ird_stat.head = ptr;
    if (ird_stat.head->magic != HEADMAGIC) {
        printf("Bad head magic\n");
        return;
    }
    printf("offset: %x\n", ird_stat.head->offset);
    ird_stat.root = ptr + ird_stat.head->offset;
    if (ird_stat.root->magic != FILEMAGIC) {
        printf("Bad root magic\n");
        return;
    }
    {
        char *eone_name = kmalloc(ird_stat.root->name_size + 1);
        memcpy(eone_name, (void *)ird_stat.root + sizeof(struct initrd_file_head_t), ird_stat.root->name_size);
        eone_name[ird_stat.root->name_size] = '\0';
        printf("RootNameCheck: %s\n", eone_name);
        kfree(eone_name);
    }
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
    strncpy(dirent->name, (uint8_t *)inner_entry + sizeof(struct initrd_file_head_t), sizeof(dirent->name));
    return dirent;
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

    struct fs_node_t *node = find_fs_node_by_ino(next_num);
    if (node == NULL) {
        node = get_new_node();
        
        strncpy(node->name, (uint8_t *)inner_entry + sizeof(struct initrd_file_head_t), sizeof(node->name));
        
        node->flags = 0;
        if(inner_entry->type == INITRDFS_FILE)
            node->flags |= FS_FILE;
        else if(inner_entry->type == INITRDFS_DIR)
            node->flags |= FS_DIRECTORY;
        
        node->length = inner_entry->size;
        node->inode = next_num;
        node->impl = 0;

        node->open = 0;
        node->close = 0;
        node->readdir = initrd_readdir;
        node->opennode = initrd_openddir;
        node->finddir = 0;

        node->ptr = 0;
    }
    return node;
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
    /*
    for (int i = 0; i < initrd_root->size; i++) {
        size_t offset = sizeof(struct initrd_file_head_t) + initrd_root->name_size + i * sizeof(size_t);

        struct initrd_file_head_t *entry_one = ptr + *((size_t *)((void *)initrd_root + offset));  // TODO: remove that pointer crazy math
        if (entry_one->magic != FILEMAGIC) {
            printf("Bad entry_one magic: %x\n", entry_one->magic);
            return;
        }
        char *eone_name = kmalloc(entry_one->name_size + 1);
        memcpy(eone_name, (void *)entry_one + sizeof(struct initrd_file_head_t), entry_one->name_size);
        eone_name[entry_one->name_size] = '\0';
        printf("/%s\n", eone_name);
        if (entry_one->type == INITRDFS_DIR) {
            for (int j = 0; j < entry_one->size; j++) {
                size_t offset_two = sizeof(struct initrd_file_head_t) + entry_one->name_size + j * sizeof(size_t);

                struct initrd_file_head_t *entry_two = ptr + (size_t) * ((uint32_t *)((void *)entry_one + offset_two));  // TODO: remove that pointer crazy math
                if (entry_two->magic != FILEMAGIC) {
                    printf("Bad entry_two magic: %x\n", entry_two->magic);
                    return;
                }
                char *etwo_name = kmalloc(entry_two->name_size + 1);
                memcpy(etwo_name, (void *)entry_two + sizeof(struct initrd_file_head_t), entry_two->name_size);
                etwo_name[entry_two->name_size] = '\0';
                printf("/%s/%s ", eone_name, etwo_name);
                kfree(etwo_name);
                if (entry_two->type == INITRDFS_DIR) {
                    printf("wtf is a directory unsupported\n");
                } else
                    printf("is file len %x\n", entry_two->size);
            }
        } else
            printf("is file len %x\n", entry_one->size);
        kfree(eone_name);
    }
    */
}

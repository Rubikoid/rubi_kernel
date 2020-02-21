#include <kernel/vfs/node.h>

struct clist_def_t nodes_list = {
    .slots = 0,
    .slot_size = sizeof(struct fs_node_t),
    .head = 0,
};

void resolve_path(char *path) {
    char *curr_name = 0;
    int i = 0;
    while (path[i] != '\0') {
    }
}

struct fs_node_t *find_fs_node_by_ino(uint32_t ino) {
    struct clist_head_t *entry;
    entry = clist_find(&dev_list, fs_node_by_ino_finder, ino);
    return (struct fs_node_t *)entry->data;
}

struct fs_node_t *get_new_node() {
    struct clist_head_t *entry;
    entry = clist_insert_after(&nodes_list, nodes_list.tail);
    return (struct fs_node_t *)entry->data;
}

uint8_t fs_node_by_ino_finder(struct clist_head_t *entry, va_list list) {
    struct fs_node_t *fs_node = (struct fs_node_t *)entry->data;
    uint32_t ino = va_arg(list, uint32_t);
    return fs_node->inode == ino;
}

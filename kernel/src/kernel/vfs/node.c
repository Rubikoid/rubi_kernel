#include <kernel/scheduler/task.h>
#include <kernel/vfs/node.h>
#include <lib/string.h>

struct clist_def_t nodes_list = {
    .slots = 0,
    .slot_size = sizeof(struct fs_node_t),
    .head = 0,
};

struct fs_node_t *resolve_path(char *path) {
    // printf("\n[FN] Check %s\n", path);
    if (path[0] != '/') {
        // printf("[FN] rel gg\n", path);
        return NULL;  // TODO: make support for relative path
    }
    char *cur_path = path + 1;  // get the first comp
    char part[NODE_NAME_SIZE];
    int i = 0;
    int k = 0;
    if(nodes_list.head == NULL)
        return NULL;
    struct fs_node_t *node = (struct fs_node_t *)nodes_list.head->data;
    // current_task->fs_status.root != NULL ? current_task->fs_status.root : (nodes_list.head != NULL ? (struct fs_node_t *)nodes_list.head->data : NULL);  // FIXME: we belive that the first elem is root. maybe we need to fix that
    if (node == NULL)
        return NULL;
    while (*cur_path != '\0' && k < 8) {
        while (*cur_path != '/' && *cur_path != '\0' && i < NODE_NAME_SIZE)
            part[i++] = *cur_path++;
        if (i >= NODE_NAME_SIZE) {
            // printf("[FN] long path gg %s\n", path);
            return NULL;
        }
        while (*cur_path == '/')
            cur_path++;
        part[i] = '\0';
        // printf("[FN] %s, %d (%s)\n", part, i, cur_path);
        if (node->finddir != 0) {  // if finddir implemented
            node = node->finddir(node, part);
            if (node == NULL)
                return NULL;
        } else {  // else, we fallback to readdir + opennode
            struct dirent_t dir;
            for (int i = 0; i < node->length + 1; i++) {
                if (node->readdir(node, i, &dir) != 0) {  // find file in directory.
                    if (!strcmp(dir.name, part)) {
                        // printf("[FN] found kek %s %x\n", dir.name, i);
                        node = node->opennode(node, i);  // two callc, may be not so optimal, but...
                        break;
                    }
                } else {  // a little hack. if we call readdir on (node->len) we should get 0, so file not found and we should die.
                    // printf("[FN] !! kek not found bug %s %x\n", part, i);
                    return NULL;
                }
            }
        }
        k++;
        i = 0;
    }
    // printf("[FN] fin %x->%x\n", k, node);
    return node;
}

struct fs_node_t *find_fs_node_by_ino(uint32_t ino) {
    struct clist_head_t *entry = 0;
    entry = clist_find(&nodes_list, fs_node_by_ino_finder, ino);
    return entry == NULL ? 0 : (struct fs_node_t *)entry->data;
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

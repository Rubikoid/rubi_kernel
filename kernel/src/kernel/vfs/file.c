#include <kernel/vfs/file.h>
#include <lib/string.h>

uint8_t file_find_path_rw(struct clist_head_t *entry, va_list list) {
    struct file_t *file = entry;
    uint8_t *path = va_arg(list, uint8_t *);
    uint16_t mode = va_arg(list, uint16_t);
    /*if(!strcmp(path, file->name) && mode == file->mod_rw)
        return 1;
    else
        return 0;*/
}

FILE *file_open(uint8_t *path, uint16_t mode_rw) {
    struct clist_heat_t *entry;
    struct file_t *file;
    struct dev_t *dev;
}

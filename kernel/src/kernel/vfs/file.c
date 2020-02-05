#include <kernel/vfs/file.h>
#include <lib/string.h>

struct clist_def_t file_list = {
    .slot_size = sizeof(struct file_t),
    .slots = 0,
    .head = 0,
};

uint32_t next_fd = 4;

uint8_t file_find_path_rw(struct clist_head_t *entry, va_list list) {
    struct file_t *file = (struct file_t *)entry;
    uint8_t *path = va_arg(list, uint8_t *);
    uint16_t mode = va_arg(list, uint16_t);
    if (!strcmp((char *)path, file->name) && mode == file->mod_rw)
        return 1;
    else
        return 0;
}

FILE *file_open(uint8_t *path, uint16_t mod_rw) {
    struct clist_head_t *entry;
    struct file_t *file;
    struct dev_t *dev;

    entry = clist_find(&file_list, file_find_path_rw, path, mod_rw);
    file = (struct file_t *)entry->data;
    if (entry != NULL)
        return &file->io_buf;

    entry = clist_insert_after(&file_list, file_list.head);
    file = (struct file_t *)entry->data;

    dev = find_dev_by_name((char *)path);
    if (dev != NULL) {
        file->dev = dev;
        if (mod_rw == 0) {
            file->io_buf.base = dev->base_r;
        } else if (mod_rw == 1) {
            file->io_buf.base = dev->base_w;
        }
    } else {
        file->dev = NULL;
        printf("Wtf file dev\n"); // TODO: filesystem
    }

    file->mod_rw = mod_rw;
    file->io_buf.fd = next_fd++;
    file->io_buf.ptr = file->io_buf.base;
    file->io_buf.eof = 0;
    file->io_buf.file = file;

    memcpy(file->name, path, strlen((char *)path) + 1);  // TODO: strncpy !

    return &file->io_buf;
}

size_t file_read(struct io_buf_t *io_buf, char *buff, uint32_t size) {
    struct file_t *file;
    file = (struct file_t *)io_buf->file;
    if(file->dev != NULL) {
        return file->dev->read_fn(&file->io_buf, buff, size);
    }
    else {
        printf("Wtf file read dev\n"); // TODO: filesystem
    }

    return 0;
}
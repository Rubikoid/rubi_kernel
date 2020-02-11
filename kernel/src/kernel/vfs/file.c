#include <kernel/asm_lib.h>
#include <kernel/memory/mmu.h>
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
    if (entry != NULL) {
        // printf("Keked old file: %x, %x, %x\n", file->io_buf.fd, &file, &file->io_buf);
        file = (struct file_t *)entry->data;
        return &file->io_buf;
    }

    entry = clist_insert_after(&file_list, file_list.tail);
    file = (struct file_t *)entry->data;

    dev = find_dev_by_name((char *)path);
    if (dev != NULL) {
        file->dev = dev;
        if (mod_rw == FILE_READ) {
            file->io_buf.base = dev->base_r;
        } else if (mod_rw == FILE_WRITE) {
            file->io_buf.base = dev->base_w;
        }
    } else {
        file->dev = NULL;
        printf("Wtf file dev\n");  // TODO: filesystem
    }

    file->mod_rw = mod_rw;
    file->io_buf.fd = next_fd++;
    file->io_buf.ptr = file->io_buf.base;
    file->io_buf.eol = 0;
    file->io_buf.eof = 0;
    file->io_buf.file = file;

    memcpy(file->name, path, strlen((char *)path) + 1);  // TODO: strncpy !
    // printf("Open new file: %x, %x, %x\n", file->io_buf.fd, &file, &file->io_buf);
    return &file->io_buf;
}

size_t file_read(struct io_buf_t *io_buf, char *buff, uint32_t size) {
    struct file_t *file;
    file = (struct file_t *)io_buf->file;
    if (file->dev != NULL) {  // TODO: check for mod
        return file->dev->read_fn(&file->io_buf, buff, size);
    } else {
        printf("Wtf file read dev\n");  // TODO: filesystem
    }

    return 0;
}

size_t file_write(struct io_buf_t *io_buf, char *buff, uint32_t size) {
    struct file_t *file = 0;

    /*if (io_buf < 0x60000) {  // hack
        void *x = get_cr3();
        x = (void *)VIRT((size_t)x);
        mmu_dump(x);
    }*/

    file = (struct file_t *)io_buf->file;
    if (file->dev != NULL) {  // TODO: check for mod
        file->dev->write_fn(&file->io_buf, buff, size);
        return 0;  // todo: return really writed data.
    } else {
        printf("Wtf file write dev\n");  // TODO: filesystem
    }
    return 0;
}

void file_ioctl(struct io_buf_t *io_buf, uint32_t cmd) {
    struct file_t *file;
    file = (struct file_t *)io_buf->file;
    if (file->dev != NULL) {
        file->dev->cmd_fn(&file->io_buf, cmd);
    } else {
        printf("Wtf file ioctl dev\n");  // TODO: filesystem
    }
}

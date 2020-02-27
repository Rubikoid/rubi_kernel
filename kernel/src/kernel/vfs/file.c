#include <kernel/asm_lib.h>
#include <kernel/memory/mmu.h>
#include <kernel/scheduler/task.h>
#include <kernel/vfs/file.h>
#include <kernel/vfs/node.h>
#include <lib/string.h>

uint8_t file_find_path_rw(struct clist_head_t *entry, va_list list) {
    struct file_t *file = (struct file_t *)entry;
    uint8_t *path = va_arg(list, uint8_t *);
    uint16_t mode = va_arg(list, uint16_t);
    //if (!strcmp((char *)path, file->name) && mode == file->mod_rw)
    //    return 1;
    //else
    //    return 0;
    return 0;
}

uint32_t file_open(uint8_t *path, uint16_t mod_rw) {
    struct fs_node_t *node = resolve_path(path);
    if (node != NULL) {
        struct clist_head_t *entry;
        struct file_t *file;
        entry = clist_insert_after(&current_task->fd_table, current_task->fd_table.tail);
        file = (struct file_t *)entry->data;
        if (node->open(node, file) != 0) {
            file->fd = current_task->next_fd++;
            file->mode = mod_rw;
            file->pos = 0;
            return file->fd;
        } else
            clist_delete(&current_task->fd_table, entry);
    }
    /*
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
    */
    return -1;
}

size_t file_read(uint32_t fd, char *buff, uint32_t size) {
    struct file_t *file;
    struct clist_head_t *entry = find_file_by_fd(fd);
    if (entry == NULL)
        return -1;
    file = entry->data;

    if (file != NULL) {
        uint32_t offset = file->pos;
        uint32_t ret = 0;
        ret = file->read(file, &offset, size, buff);
        file->pos = offset;
        return ret;
    }
    /*if (file->dev != NULL) {  // TODO: check for mod
        return file->dev->read_fn(&file->io_buf, buff, size);
    } else {
        printf("Wtf file read dev\n");  // TODO: filesystem
    }*/

    return -1;
}

size_t file_write(uint32_t fd, char *buff, uint32_t size) {
    struct file_t *file = 0;
    struct clist_head_t *entry = find_file_by_fd(fd);
    if (entry == NULL)
        return -1;
    file = entry->data;

    if (file != NULL) {
        uint32_t offset = file->pos;
        uint32_t ret = 0;
        ret = file->write(file, &offset, size, buff);
        file->pos = offset;
        return ret;
    }

    /*if (io_buf < 0x60000) {  // hack
        void *x = get_cr3();
        x = (void *)VIRT((size_t)x);
        mmu_dump(x);
    }*/

    /*
    file = (struct file_t *)io_buf->file;
    if (file->dev != NULL) {  // TODO: check for mod
        file->dev->write_fn(&file->io_buf, buff, size);
        return 0;  // todo: return really writed data.
    } else {
        printf("Wtf file write dev\n");  // TODO: filesystem
    }
    */

    return -1;
}

uint32_t file_ioctl(uint32_t fd, uint32_t cmd, uint32_t subcmd) {
    struct file_t *file;
    struct clist_head_t *entry = find_file_by_fd(fd);
    if (entry == NULL)
        return -1;
    file = entry->data;

    if (file != NULL) {
        return file->ioctl(file, cmd, subcmd);
    }
    return -1;
}

struct file_t *find_file_by_fd(uint32_t fd) {
    struct clist_head_t *entry = 0;
    entry = clist_find(&current_task->fd_table, file_by_fd_finder, fd);
    return entry == NULL ? 0 : (struct file_t *)entry->data;
}

uint8_t file_by_fd_finder(struct clist_head_t *entry, va_list list) {
    struct file_t *file = (struct file_t *)entry->data;
    uint32_t fd = va_arg(list, uint32_t);
    return file->fd == fd;
}

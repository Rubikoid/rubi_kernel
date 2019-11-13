#include <kernel/defines.h>
#include <lib/slist.h>
#include <types.h>

#ifndef KERNEL_MEMORY_HEAP_H_
#define KERNEL_MEMORY_HEAP_H_

#define KHEAP_SIZE 2048

struct kheap_entry_t {
    struct slist_head_t head;
    size_t addr;
    size_t size;
    uint8_t is_busy;
};

// kernel heap data
extern struct kheap_entry_t kheap_blocks[KHEAP_SIZE];
extern struct slist_def_t kheap_list;

extern void init_kheap_pages();
extern void *kmalloc(size_t count);
extern void kfree(void *ptr);

extern void *kmalloc_a(size_t count, uint32_t align);
extern void kfree_a(void *ptr);

extern void kheap_dump(struct slist_def_t *list);
#endif

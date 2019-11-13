#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <lib/slist.h>
#include <lib/stdio.h>
#include <types.h>

struct kheap_entry_t kheap_blocks[KHEAP_SIZE];
struct slist_def_t kheap_list = {
    .base = (size_t)kheap_blocks,
    .slots = KHEAP_SIZE,
    .slot_size = sizeof(struct kheap_entry_t),
    .head = NULL,
    .tail = NULL};

size_t kheap_start_addr = VIRT(TABLE_SIZE * KERNEL_LOWER_TABLES);
size_t kheap_size = 2 * TABLE_SIZE;  // 2 * 4mb (at least one page)

void init_kheap_pages() {
    // alloc pages for kheap
    /*kheap_start_addr = (size_t)alloc_page();             // alloc at least one page for kernel heap;
    for (int i = 0; i < (kheap_size / 4096) - 1; i++) {  // alloc other pages for kheap, -1 because we allocated one yet
        alloc_page();
    }*/
}

void *kmalloc(size_t size) {
    struct kheap_entry_t *current_entry = NULL;
    struct slist_head_t *current_sl = NULL;
    for (current_sl = kheap_list.head; current_sl != NULL; current_sl = current_sl->next) {
        current_entry = (struct kheap_entry_t *)current_sl->data;
        if (current_entry->is_busy)
            continue;
        if (current_entry->size < size) {  // if we fund block, that smaller than we need
            size_t size_diff = size - current_entry->size;
            ;
            // steal diff from left
            if (current_sl->prev != NULL) {
                struct kheap_entry_t *prev = NULL;
                prev = (struct kheap_entry_t *)current_sl->prev->data;
                if (!prev->is_busy) {
                    prev->size -= size_diff;
                    current_entry->addr -= size_diff;
                    current_entry->size += size_diff;
                    if (prev->size == 0)
                        slist_delete(&kheap_list, current_sl->prev);
                    current_entry->is_busy = 1;
                    return (void *)current_entry->addr;
                }
            }

            // just forget diff
            if (current_sl->next == NULL) {
                size_t kheap_alloc_addr = current_entry->addr + current_entry->size;
                if (kheap_alloc_addr + size_diff < kheap_start_addr + kheap_size) {
                    current_entry->size += size_diff;
                    current_entry->is_busy = 1;
                    return (void *)current_entry->addr;
                }
            }
        } else {  // if we found block, that bigger than we need
            current_entry->is_busy = 1;
            size_t size_diff = current_entry->size - size;
            struct kheap_entry_t *next = NULL;
            uint8_t next_busy = 0;
            if (current_sl->next != NULL) {  // if we have right sibling, we can try to give him our mem
                next = (struct kheap_entry_t *)current_sl->next->data;
                next_busy = next->is_busy;
                if (!next_busy) {
                    current_entry->size -= size_diff;
                    next->addr -= size_diff;
                    next->size += size_diff;
                }
            }
            if (next == NULL || next_busy) {
                next = (struct kheap_entry_t *)slist_insert_after(&kheap_list, current_sl);
                if (next != NULL) {  // why we do this check
                    current_entry->size -= size_diff;
                    next->is_busy = 0;
                    next->addr = current_entry->addr + current_entry->size;
                    next->size = size_diff;
                }
            }

            return (void *)current_entry->addr;
        }
    }

    // so if we here. we have no free blocks in list, so add new to the end
    size_t kheap_alloc_addr = kheap_start_addr;
    if (kheap_list.tail) {
        current_entry = (struct kheap_entry_t *)kheap_list.tail->data;
        kheap_alloc_addr += current_entry->addr + current_entry->size;
    }
    if (kheap_alloc_addr >= kheap_start_addr + kheap_size) {
        return 0;
    }

    current_sl = slist_insert_after(&kheap_list, kheap_list.tail);
    current_entry = (struct kheap_entry_t *)current_sl->data;
    current_entry->is_busy = 1;
    current_entry->addr = kheap_alloc_addr;
    current_entry->size = size;

    return (void *)current_entry->addr;
}

void kfree(void *ptr) {
    struct kheap_entry_t *current_entry = NULL;
    struct slist_head_t *current_sl = NULL;
    for (current_sl = kheap_list.head; current_sl != NULL; current_sl = current_sl->next) {
        current_entry = (struct kheap_entry_t *)current_sl->data;
        if (!current_entry->is_busy)
            continue;
        if (current_entry->addr == (size_t)ptr) {
            struct kheap_entry_t *prev = current_sl->prev != NULL ? (struct kheap_entry_t *)current_sl->prev->data : NULL;
            struct kheap_entry_t *next = current_sl->next != NULL ? (struct kheap_entry_t *)current_sl->next->data : NULL;

            current_entry->is_busy = 0;

            if (prev != NULL && !prev->is_busy) {
                prev->size += current_entry->size;
                slist_delete(&kheap_list, current_sl);
                current_sl = &prev->head;
                current_entry = prev;
            }

            if (next != NULL && !next->is_busy) {
                current_entry->size += next->size;
                slist_delete(&kheap_list, &next->head);
            }

            return;
        }
    }
}

void *kmalloc_a(size_t count, uint32_t align) {
    void *p = kmalloc(count + align + sizeof(size_t) - 1);
    if (p == NULL)
        return NULL;
    void *ptr = (void *)((((size_t)p + sizeof(void *) + align - 1) & ~(align - 1)));
    *((void **)((size_t)ptr - sizeof(void *))) = p;
    return ptr;
}

void kfree_a(void *ptr) {
    void *p = *((void **)((size_t)ptr - sizeof(void *)));
    kfree(p);
    return;
}

void kheap_dump(struct slist_def_t *list) {
    printf("-- kheap slist dump\n");
    struct kheap_entry_t *current_entry = NULL;
    for (struct slist_head_t *elem = list->head; elem != NULL; elem = elem->next) {
        current_entry = (struct kheap_entry_t *)elem->data;
        printf("    t=%x p=%x n=%x addr=%x size=%x busy=%x\n", elem, elem->prev, elem->next, current_entry->addr, current_entry->size, current_entry->is_busy);
    }
}

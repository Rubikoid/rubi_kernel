#include <types.h>
#include <lib/stdio.h>
#include <lib/slist.h>
#include <kernel/utils/memory.h>

volatile struct page_directory_entry_t *kernel_page_directory;
volatile struct page_table_entry_t *kernel_page_table;

struct kheap_entry_t kheap_blocks[KHEAP_SIZE];
struct slist_def_t kheap_list = {
    .base = (size_t) kheap_blocks,
    .slots = KHEAP_SIZE,
    .slot_size = sizeof(struct kheap_entry_t),
    .head = NULL,
    .tail = NULL
};

size_t kheap_start_addr;
size_t kheap_size = 2 * 4096; // 2 * 4kb (at least one page)

void init_memory_manager() {
    //last_page_ID = VIRT_BASE + KERNEL_TABLE_COUNT * TABLE_SIZE + PAGE_SIZE;  // additional page for why not
    //page_directory_ptr = (volatile uint32_t *)&boot_page_directory;
    //page_table_ptr = (volatile uint32_t *)&boot_page_table;

    kernel_page_directory = (struct page_directory_entry_t *) &boot_page_directory;
    kernel_page_table = (struct page_table_entry_t *) &boot_page_table;
}

void mmu_dump() {
    volatile struct page_directory_entry_t *cur_dir = NULL;
    volatile struct page_table_entry_t *cur_table = NULL;
    for(int i=767; i<775;i++) {
        cur_dir = &kernel_page_directory[i];
        if(1 || cur_dir->present) {
            cur_table = (volatile struct page_table_entry_t *) VIRT(cur_dir->page_table_addr << 12);
            printf("[%u] Addr: %x [%x] [%x->%x->] s : %x rw: %x p: %x\n", 
                i, 
                cur_dir->page_table_addr,
                VIRT(cur_dir->page_table_addr << 12),
                (i << 22) + (cur_table[0].page_phys_addr << 10),
                (i << 22) + (cur_table[1].page_phys_addr << 10),
                // (i << 22) + (cur_table[2].page_phys_addr << 10), 
                // (i << 22) + (cur_table[1023].page_phys_addr << 10), 
                cur_dir->user_supervisor, 
                cur_dir->read_write, 
                cur_dir->present);

        }
    }
}

void *alloc_page() {
    /* uint32_t PDE_index = (last_page_ID) >> 22;  // TODO: use more complex way for computing indexes and addrs
    uint32_t PTE_index = ((last_page_ID & TABLE_BIT_FIELD) >> 12);
    uint32_t PDE = page_directory_ptr[PDE_index];
    printf("Allocating new page with PDEi: %u PTEi: %x PDE: %x\n", PDE_index, PTE_index, PDE);
    page_directory_ptr[PDE_index] |= 0x003;
    ((uint32_t *)(VIRT_BASE + (PDE & PDTE_BIT_FIELD)))[PTE_index] = ((last_page_ID - VIRT_BASE) & PDTE_BIT_FIELD) + 0x003;
    last_page_ID += PAGE_SIZE;

    return (void *)(last_page_ID - PAGE_SIZE); */
    return 0;
}

void free_page(void *page_addr_in) {
    if (page_addr_in == NULL)
        return;
    /*size_t page_addr = (size_t)page_addr_in;
    uint32_t PDE_index = (page_addr) >> 22;
    uint32_t PTE_index = ((page_addr & TABLE_BIT_FIELD) >> 12);
    uint32_t PDE = page_directory_ptr[PDE_index];

    ((uint32_t *)(VIRT_BASE + (PDE & PDTE_BIT_FIELD)))[PTE_index] = 0;

    if (last_page_ID - page_addr == PAGE_SIZE)  // TODO: make normal memory controller, not that stillborn
        last_page_ID -= PAGE_SIZE; */
    return;
}

void init_kheap_pages() {
    // alloc pages for kheap
    kheap_start_addr = (size_t) alloc_page(); // alloc at least one page for kernel heap;
    for(int i = 0; i < (kheap_size / 4096) - 1; i++) { // alloc other pages for kheap, -1 because we allocated one yet
        alloc_page();
    }
}

void *kmalloc(size_t size) {
    struct kheap_entry_t* current_entry = NULL;
    struct slist_head_t* current_sl = NULL;
    for(current_sl = kheap_list.head; current_sl != NULL; current_sl = current_sl->next) {
        current_entry = (struct kheap_entry_t*)current_sl->data;
        if(current_entry->is_busy)
            continue;
        if(current_entry->size < size) { // if we fund block, that smaller than we need
            size_t size_diff = size - current_entry->size;;
            // steal diff from left
            if(current_sl->prev != NULL) {
                struct kheap_entry_t* prev = NULL;
                prev = (struct kheap_entry_t*) current_sl->prev->data;
                if(!prev->is_busy) {
                    prev->size -= size_diff;
                    current_entry->addr -= size_diff;
                    current_entry->size += size_diff;
                    if(prev->size == 0)
                        slist_delete(&kheap_list, current_sl->prev);
                    current_entry->is_busy = 1;
                    return (void *) current_entry->addr; 
                }
            }

            // just forget diff
            if(current_sl->next == NULL) {
                size_t kheap_alloc_addr = current_entry->addr + current_entry->size;
                if(kheap_alloc_addr + size_diff < kheap_start_addr + kheap_size) {
                    current_entry->size += size_diff;
                    current_entry->is_busy = 1;
                    return (void *) current_entry->addr;
                }

            }
        }
        else { // if we found block, that bigger than we need
            current_entry->is_busy = 1;
            size_t size_diff = current_entry->size - size;
            struct kheap_entry_t* next = NULL;
            uint8_t next_busy = 0;
            if(current_sl->next != NULL) { // if we have right sibling, we can try to give him our mem
                next = (struct kheap_entry_t*)current_sl->next->data;
                next_busy = next->is_busy;
                if(!next_busy) {
                    current_entry->size -= size_diff;
                    next->addr -= size_diff;
                    next->size += size_diff;
                }
            }
            if(next == NULL || next_busy) {
                next = (struct kheap_entry_t*) slist_insert_after(&kheap_list, current_sl);
                if(next != NULL) { // why we do this check
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
    if(kheap_list.tail) {
        current_entry = (struct kheap_entry_t*)kheap_list.tail->data;
        kheap_alloc_addr += current_entry->addr + current_entry->size;
    }
    if(kheap_alloc_addr >= kheap_start_addr + kheap_size) {
        return 0;
    }

    current_sl = slist_insert_after(&kheap_list, kheap_list.tail);
    current_entry = (struct kheap_entry_t*)current_sl->data;
    current_entry->is_busy = 1;
    current_entry->addr = kheap_alloc_addr;
    current_entry->size = size;

    return (void *) current_entry->addr;
}

void kfree(void *ptr) {
    struct kheap_entry_t* current_entry = NULL;
    struct slist_head_t* current_sl = NULL;
    for(current_sl = kheap_list.head; current_sl != NULL; current_sl = current_sl->next) {
        current_entry = (struct kheap_entry_t*)current_sl->data;
        if(!current_entry->is_busy)
            continue;
        if(current_entry->addr == (size_t) ptr) {
            struct kheap_entry_t* prev = current_sl->prev != NULL ? (struct kheap_entry_t*) current_sl->prev->data : NULL;
            struct kheap_entry_t* next = current_sl->next != NULL ? (struct kheap_entry_t*) current_sl->next->data : NULL;

            current_entry->is_busy = 0;

            if(prev != NULL && !prev->is_busy) {
                prev->size += current_entry->size;
                slist_delete(&kheap_list, current_sl);
                current_sl = &prev->head;
                current_entry = prev;
            }

            if(next != NULL && !next->is_busy) {
                current_entry->size += next->size;
                slist_delete(&kheap_list, &next->head);
            }

            return;
        }
    }
}

void kheap_dump(struct slist_def_t *list) {
    printf("-- kheap slist dump\n");
    struct kheap_entry_t* current_entry = NULL;
    for(struct slist_head_t *elem = list->head; elem != NULL; elem = elem->next) {
        current_entry = (struct kheap_entry_t*)elem->data;
        printf("    t=%x p=%x n=%x addr=%x size=%x busy=%x\n", elem, elem->prev, elem->next, current_entry->addr, current_entry->size, current_entry->is_busy);
    }
}

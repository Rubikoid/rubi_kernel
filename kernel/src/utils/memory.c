#include <types.h>
#include <kernel/utils/memory.h>

size_t last_page_ID;
volatile uint32_t *page_directory_ptr;
volatile uint32_t *page_table_ptr;

void init_memory_manager() {
    last_page_ID = VIRT_BASE + KERNEL_TABLE_COUNT * TABLE_SIZE + PAGE_SIZE;  // additional page for why not
    page_directory_ptr = (volatile uint32_t *)&boot_page_directory;
    page_table_ptr = (volatile uint32_t *)&boot_page_table;
}

void *alloc_page() {
    uint32_t PDE_index = (last_page_ID) >> 22;  // TODO: use more complex way for computing indexes and addrs
    uint32_t PTE_index = ((last_page_ID & TABLE_BIT_FIELD) >> 12);
    uint32_t PDE = page_directory_ptr[PDE_index];

    page_directory_ptr[PDE_index] |= 0x003;
    ((uint32_t *)(VIRT_BASE + (PDE & PDTE_BIT_FIELD)))[PTE_index] = ((last_page_ID - VIRT_BASE) & PDTE_BIT_FIELD) + 0x003;
    last_page_ID += PAGE_SIZE;

    return (void *)(last_page_ID - PAGE_SIZE);
}

void free_page(void *page_addr_in) {
    if (page_addr_in == NULL)
        return;
    size_t page_addr = (size_t)page_addr_in;
    uint32_t PDE_index = (page_addr) >> 22;
    uint32_t PTE_index = ((page_addr & TABLE_BIT_FIELD) >> 12);
    uint32_t PDE = page_directory_ptr[PDE_index];

    ((uint32_t *)(VIRT_BASE + (PDE & PDTE_BIT_FIELD)))[PTE_index] = 0;

    if (last_page_ID - page_addr == PAGE_SIZE)  // TODO: make normal memory controller, not that stillborn
        last_page_ID -= PAGE_SIZE;
    return;
}

uint8_t last_mem_page_id = -1;
struct mem_info_t allocated_pages[128] = {0};  // TODO: make define or normal MM

void *alloc_mem(size_t count) {
    if (count > PAGE_SIZE)
        return NULL;
    struct mem_info_t *last_page = &allocated_pages[last_mem_page_id];
    if (count > last_page->free_mem_in_page) {
        last_mem_page_id += 1;
        last_page = &allocated_pages[last_mem_page_id];
        last_page->pointer = alloc_page();
        last_page->free_mem_in_page = PAGE_SIZE - count;
        return last_page->pointer;
    } else {
        void *ret = 0;
        ret = last_page->pointer + (PAGE_SIZE - last_page->free_mem_in_page);
        last_page->free_mem_in_page -= count;
        return ret;
    }
    return NULL;
}

void free_mem(void *ptr, size_t count) {  // FIXME: Create fix for what __genious__ solution with count;
    uint32_t base_addr = ((uint32_t)ptr & PDTE_BIT_FIELD);
    for (int i = 0; i <= last_mem_page_id; i++) {
        if (allocated_pages[i].pointer == base_addr) {
            struct mem_info_t *last_page = &allocated_pages[i];
            last_page->free_mem_in_page += count;
            if (last_page->free_mem_in_page == PAGE_SIZE && i == last_mem_page_id) {
                free_page(last_page->pointer);
                last_page->pointer = 0;
                last_mem_page_id -= 1;
            }
            break;
        }
    }

    //check if we have more empty pages
    if (allocated_pages[last_mem_page_id].free_mem_in_page == PAGE_SIZE) {
        free_page(allocated_pages[last_mem_page_id].pointer);
        allocated_pages[last_mem_page_id].pointer = 0;
        last_mem_page_id -= 1;
    }
}

uint32_t memcpy(uint8_t *from, uint8_t *to, size_t count) {
    int i = 0;
    for (; i < count; i++) {
        to[i] = from[i];
    }
    return i;
}

#include <stddef.h>
#include <stdint.h>

#define DEBUG_ASM asm("xchg %bx, %bx");

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
    uint32_t PDE_index = (last_page_ID) >> 22;
    uint32_t PTE_index = ((last_page_ID & TABLE_BIT_FIELD) >> 12);
    uint32_t PDE = page_directory_ptr[PDE_index];
    
    page_directory_ptr[PDE_index] |= 0x003;
    ((uint32_t *)(VIRT_BASE + (PDE & PDTE_BIT_FIELD)))[PTE_index] = ((last_page_ID - VIRT_BASE) & PDTE_BIT_FIELD) + 0x003;
    last_page_ID += PAGE_SIZE;
    
    DEBUG_ASM;
    return (void *)(last_page_ID - PAGE_SIZE);
}

void free_page(void *page_addr_in) {
	if(page_addr_in == NULL)
		return;
    size_t page_addr = (size_t) page_addr_in;
    uint32_t PDE_index = (page_addr) >> 22;
    uint32_t PTE_index = ((page_addr & TABLE_BIT_FIELD) >> 12);
    uint32_t PDE = page_directory_ptr[PDE_index];

    ((uint32_t *)(VIRT_BASE + (PDE & PDTE_BIT_FIELD)))[PTE_index] = 0;

    if(last_page_ID - page_addr == PAGE_SIZE) // TODO: make normal memory controller, not that stillborn
        last_page_ID -= PAGE_SIZE;
    return;
}

uint16_t free_mem_in_page = 0;
uint8_t last_mem_page_id = 0;
void *allocated_pages[128] = {0}; // TODO: make define or normal MM

void *alloc_mem(size_t count) {
    if(count > free_mem_in_page) {
    	
    }
    return NULL;
}

void free_mem(void *ptr) {

}

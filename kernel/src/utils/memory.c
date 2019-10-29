#include <stddef.h>
#include <stdint.h>

#define DEBUG_ASM asm("xchg %bx, %bx");

#include <kernel/utils/memory.h>

size_t last_page_ID;
volatile uint32_t *page_directory_ptr;
volatile uint32_t *page_table_ptr;


void init_memory_manager() {
    last_page_ID = VIRT_BASE + KERNEL_TABLE_COUNT * TABLE_SIZE + PAGE_SIZE;  // additional page for why not
    page_directory_ptr = (volatile uint32_t*) &boot_page_directory;
    page_table_ptr = (volatile uint32_t*) &boot_page_table;
}

void *alloc_page() {
    DEBUG_ASM;
    uint32_t PDEi = (last_page_ID) >> 22;
    uint32_t PTEi = ((last_page_ID & TABLE_BIT_FIELD) >> 12);
    uint32_t PDE = page_directory_ptr[PDEi];
    page_directory_ptr[PDEi] |= 0x003;
    DEBUG_ASM;
    ((uint32_t *)((size_t) VIRT_BASE + (PDE & PDTE_BIT_FIELD)))[PTEi] |= 0x003;
    DEBUG_ASM;
    return 0;
}

void free_page() {

}

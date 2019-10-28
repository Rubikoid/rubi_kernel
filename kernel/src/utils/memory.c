#include <stddef.h>
#include <stdint.h>

#define DEBUG_ASM asm("xchg %bx, %bx");

#include <kernel/utils/memory.h>

uint32_t last_page_ID;

void init_memory_manager() {
    last_page_ID = VIRT_BASE + KERNEL_TABLE_COUNT * TABLE_SIZE + PAGE_SIZE;  // additional page for why not
}

void *alloc_page() {
    DEBUG_ASM;
    uint32_t PDEi = (last_page_ID) >> 22;
    uint32_t PTEi = ((last_page_ID & TABLE_BIT_FIELD) >> 12);
    
    DEBUG_ASM;
    //boot_page_table[((PDE & PDTE_BIT_FIELD)) + PTEi] |= 0x003;
    DEBUG_ASM;
}

void free_page() {
}

#include <types.h>

#include <kernel/asm_lib.h>
#include <kernel/defines.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <kernel/scheduler/task.h>
#include <kernel/serial/serial.h>
#include <kernel/utils/DT/dt.h>
#include <kernel/utils/DT/int.h>
#include <kernel/utils/utils.h>
#include <kernel/vga/vga.h>
#include <lib/clist.h>
#include <lib/slist.h>
#include <lib/stdio.h>
#include <lib/string.h>

//#if defined(__linux__)
//	#error "This code must be compiled with a cross-compiler"
//#elif !defined(__i386__)
//	#error "This code must be compiled with an x86-elf compiler"
//#endif

typedef struct page_directory_entry_t *pdep_t;
typedef struct page_table_entry_t *ptep_t;

void test() {
    DEBUG_ASM;
    for (uint32_t i = 0; i < 1000; i++)
        printf("[%u] ", i);
    __asm__("movl $1, %eax");
    __asm__("int $0x80");
}

void kernel_main(struct multiboot_t *multiboot, void *kstack) {
    init_com(0);
    term_init();
    term_print("[" G_GREEN "OK" G_WHITE "] Serial and terminal\n");

    init_memory_manager();
    term_print("[" G_GREEN "OK" G_WHITE "] MemoryManager\n");

    gdt_init();
    idt_init();
    pic_enable();
    term_print("[" G_GREEN "OK" G_WHITE "] GDT, IDT, PIC\n");

    multiboot = (struct multiboot_t *)(((size_t)multiboot) + 0xC0000000);  // make virtual ptr to multiboot structure

    term_print("[" G_GREEN "OK" G_WHITE "] RubiKernel " KERNEL_VERSION ": Init!\n");
    printf("Multiboot: 0x%x; StackStart: 0x%x; Mem_upper: %u\n", multiboot, kstack, multiboot->mem_upper);

    pdep_t pde1 = create_page_directory();
    ptep_t pte1 = create_page_table(2);
    bind_table(pde1, pte1, 0x0);
    alloc_page(pte1, 0x0);
    alloc_page(pte1, 0x0 + PAGE_SIZE);
    struct task_mem_t t = {
        .pages = 0,
        .pages_count = 2,
        .page_dir = pde1,
        .page_table = pte1,
    };
    task_create(1, ddd, &t);
    /*
    pdep_t pde1 = create_page_directory();
    ptep_t pte1 = create_page_table(2);
    bind_table(pde1, pte1, 0x0);
    void *a1 = alloc_page(pte1, 0x0);
    void *b1 = alloc_page(pte1, 0x0 + PAGE_SIZE);

    pdep_t pde2 = create_page_directory();
    ptep_t pte2 = create_page_table(2);
    bind_table(pde2, pte2, 0x0);
    void *a2 = alloc_page(pte2, 0x0);
    void *b2 = alloc_page(pte2, 0x0 + PAGE_SIZE);

    printf("[" G_GREEN "OK" G_WHITE "] Allocated (%x,%x), (%x,%x)\n", a1, b1, a2, b2);
    printf("[" G_LGRAY "OK" G_WHITE "] Switching to %x\n", pde1);
    enable_paging((void *)PHYS((size_t)pde1));
    printf("[" G_GREEN "OK" G_WHITE "] Switched to %x\n", pde1);
    *((uint32_t *)10) = 0x1337;

    printf("[" G_LGRAY "OK" G_WHITE "] Switching to %x\n", pde2);
    enable_paging((void *)PHYS((size_t)pde2));
    printf("[" G_GREEN "OK" G_WHITE "] Switched to %x\n", pde2);
    *((uint32_t *)10) = 0xdeadbeef;

    printf("[" G_LGRAY "OK" G_WHITE "] Switching to %x\n", pde1);
    enable_paging((void *)PHYS((size_t)pde1));
    printf("[" G_GREEN "OK" G_WHITE "] Switched to %x\n", pde1);
    printf("[" G_GREEN "OK" G_WHITE "] Scan value: %x\n", *((uint32_t *)10));

    printf("[" G_LGRAY "OK" G_WHITE "] Switching to %x\n", pde2);
    enable_paging((void *)PHYS((size_t)pde2));
    printf("[" G_GREEN "OK" G_WHITE "] Switched to %x\n", pde2);
    printf("[" G_GREEN "OK" G_WHITE "] Scan value: %x\n", *((uint32_t *)10)); 
*/
    while (1) { printf("k"); halt(); }
    return;
}

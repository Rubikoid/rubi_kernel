#include <types.h>

#include <kernel/asm_lib.h>
#include <kernel/defines.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <kernel/serial/serial.h>
#include <kernel/utils/DT/dt.h>
#include <kernel/utils/DT/int.h>
#include <kernel/utils/utils.h>
#include <kernel/vga/vga.h>
#include <lib/slist.h>
#include <lib/stdio.h>

//#if defined(__linux__)
//	#error "This code must be compiled with a cross-compiler"
//#elif !defined(__i386__)
//	#error "This code must be compiled with an x86-elf compiler"
//#endif

typedef struct page_directory_entry_t *pdep_t;
typedef struct page_table_entry_t *ptep_t;

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

    mmu_dump();

/*
    pdep_t pde = create_page_directory();
    ptep_t pte = create_page_table(2);
    bind_addr(pde, pte, 0x0, (void *)PHYS_TASKS_SPACE_START);
    bind_addr(pde, pte, (void *)(0x0 + 0x1000), (void *)PHYS_TASKS_SPACE_START + 0x1000);
    enable_paging((void *)PHYS((size_t)pde));
*/
    // kheap_dump(&kheap_list);

    while (1) halt();
    // printf("sizeof(unlong)=%u\n", sizeof(unsigned long));
    //abort("ABORT: test\n");
    //term_print_int(last_page_ID, 16);
    //term_print("\n\n");

    // term_print("End!\n");

    return;
}

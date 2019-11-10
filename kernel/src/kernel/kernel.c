#include <types.h>

#include <kernel/asm_lib.h>
#include <kernel/defines.h>
#include <kernel/utils/DT/dt.h>
#include <kernel/utils/DT/int.h>
#include <kernel/utils/memory.h>
#include <kernel/utils/utils.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>

//#if defined(__linux__)
//	#error "This code must be compiled with a cross-compiler"
//#elif !defined(__i386__)
//	#error "This code must be compiled with an x86-elf compiler"
//#endif

void kernel_main(struct multiboot_t* multiboot, void* kstack) {
    init_memory_manager();

    gdt_init();
    idt_init();

    term_init();
    multiboot = (struct multiboot_t*)(((size_t)multiboot) + 0xC0000000);  // make virtual ptr to multiboot structure

    term_print("[" G_GREEN "OK" G_WHITE "] RubiKernel " KERNEL_VERSION ": Init!\n");
    printf("Multiboot: 0x%x\n", multiboot);
    printf("StackStart: 0x%x\n", kstack);
    printf("Mem_lower: %u\nMem_upper: %u\n", multiboot->mem_lower, multiboot->mem_upper);
    printf("Last page id: 0x%x \n", last_page_ID);
    int a = *((int*)0x1337);
    // printf("sizeof(unlong)=%u\n", sizeof(unsigned long));
    //abort("ABORT: test\n");
    //term_print_int(last_page_ID, 16);
    //term_print("\n\n");

    // term_print("End!\n");

    return;
}

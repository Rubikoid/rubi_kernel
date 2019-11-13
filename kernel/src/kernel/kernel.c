#include <types.h>

#include <kernel/asm_lib.h>
#include <kernel/defines.h>
#include <kernel/utils/DT/dt.h>
#include <kernel/utils/DT/int.h>
#include <kernel/utils/memory.h>
#include <kernel/utils/utils.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>
#include <lib/slist.h>

//#if defined(__linux__)
//	#error "This code must be compiled with a cross-compiler"
//#elif !defined(__i386__)
//	#error "This code must be compiled with an x86-elf compiler"
//#endif

void kernel_main(struct multiboot_t* multiboot, void* kstack) {
    term_init();
    term_print("[" G_GREEN "OK" G_WHITE "] Terminal\n");

    init_memory_manager();
    term_print("[" G_GREEN "OK" G_WHITE "] MemoryManager\n");
    
    gdt_init();
    idt_init();
    pic_enable();
    term_print("[" G_GREEN "OK" G_WHITE "] GDT, IDT, PIC\n");

    multiboot = (struct multiboot_t*)(((size_t)multiboot) + 0xC0000000);  // make virtual ptr to multiboot structure

    term_print("[" G_GREEN "OK" G_WHITE "] RubiKernel " KERNEL_VERSION ": Init!\n");
    printf("Multiboot: 0x%x; StackStart: 0x%x; Mem_upper: %u\n", multiboot, kstack, multiboot->mem_upper);
    printf("Last page id: 0x%x;\n", last_page_ID);
    
    // slist_test();
    void *x = kmalloc(10);
    void *y = kmalloc(10);
    void *z = kmalloc(10);
    
    kheap_dump(&kheap_list);

    kfree(x);

    kheap_dump(&kheap_list);

    kfree(y);
    
    kheap_dump(&kheap_list);

    kfree(z);

    kheap_dump(&kheap_list);

    x = kmalloc(5);
    kheap_dump(&kheap_list);
    y = kmalloc(50);
    kheap_dump(&kheap_list);
    
    while(1) halt();
    // printf("sizeof(unlong)=%u\n", sizeof(unsigned long));
    //abort("ABORT: test\n");
    //term_print_int(last_page_ID, 16);
    //term_print("\n\n");

    // term_print("End!\n");

    return;
}

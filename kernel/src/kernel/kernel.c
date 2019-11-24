#include <types.h>

#include <kernel/DT/dt.h>
#include <kernel/DT/int.h>
#include <kernel/DT/syscall.h>
#include <kernel/asm_lib.h>
#include <kernel/defines.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <kernel/scheduler/task.h>
#include <kernel/serial/serial.h>
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
    uint32_t e = 0;
    for (uint32_t i = 0; i < 100; i++) {
        e += i;
    }
    fsyscall(SYSCALL_EXIT, 0, 0, 0);
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

    {
        pdep_t pde1 = create_page_directory();
        ptep_t pte1 = create_page_table(1);
        bind_table(pde1, pte1, 0x0);
        alloc_page(pte1, 0x0);
        struct task_mem_t t = {
            .pages = 0,
            .pages_count = 2,
            .page_dir = pde1,
            .page_table = pte1,
        };
        task_create(1, test, &t);
    }

    {
        struct task_mem_t t = {
            .pages = 0,
            .pages_count = 0,
            .page_dir = kernel_page_directory,
            .page_table = kernel_page_table,
        };
        task_create(2, test, &t);
    }


    {
        struct task_mem_t t = {
            .pages = 0,
            .pages_count = 0,
            .page_dir = kernel_page_directory,
            .page_table = kernel_page_table,
        };
        task_create(3, test, &t);
    }

    /*
    uint32_t eip = 0;  // very shit hack, how not to die, if there are no tasks
    if (eip != 0) {
    eip_setted:;
    } else {
        asm volatile("1: lea 1b, %0;"
                     : "=a"(eip));
        goto eip_setted;
    } */

    while (1) {
        printf("{inf}");
        halt();
    }  // infiloop
    return;
}

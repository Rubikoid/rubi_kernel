#include <types.h>

#include <kernel/DT/dt.h>
#include <kernel/DT/int.h>
#include <kernel/DT/syscall.h>
#include <kernel/asm_lib.h>
#include <kernel/defines.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <kernel/messages.h>
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

void infiloop() {
    while (1) {
        printf(MSG_INFINITY);
        halt();
    }  // infiloop
}

void test1() {
    struct message_t msg_out = {
        .type = 1,
        .len = 4,
        .data = kmalloc(4),
    };
    memcpy(msg_out.data, "abc", 4);
    ksend(2, &msg_out);
    printf("Sended message type: %x with len: %x and data: %s\n", msg_out.type, msg_out.len, msg_out.data);
    texit();
}

void test2() {
    struct message_t msg_out;
    ktrecv(&msg_out);
    printf("Recived message type: %x with len: %x and data: %s\n", msg_out.type, msg_out.len, msg_out.data);
    texit();
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
    disable_int();

    /*
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
        task_create(1, test, &t)->status = TASK_RUNNING;
    }
    task_create(3, test, NULL)->status = TASK_RUNNING;
    */
    task_create(2, test2, NULL)->status = TASK_RUNNING;
    task_create(1, test1, NULL)->status = TASK_RUNNING;

    task_create(0, infiloop, NULL)->status = TASK_RUNNING;
    enable_int();

    while (1) {
        printf(MSG_INFINITY);
        halt();
    }  // infiloop
    return;
}

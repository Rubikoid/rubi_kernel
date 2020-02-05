#include <types.h>

#include <kernel/DT/dt.h>
#include <kernel/DT/int.h>
#include <kernel/DT/syscall.h>
#include <kernel/asm_lib.h>
#include <kernel/defines.h>
#include <kernel/kthread/dq.h>
#include <kernel/kthread/ktasks.h>
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

void infiloop();
void create_kernel_tasks();

void test1() {
    printf("Running test1\n");
    uint16_t tid = syscall_gettid();
    struct message_t msg_out = {
        .type = 1,
        .len = 2,
        .data = kmalloc(2),
    };
    (*(uint16_t *)msg_out.data) = tid;
    ksend(ktasks[KERNEL_DQ_TASK_ID]->tid, &msg_out);

    struct message_t msg_in;
    syscall_krecv(&msg_in);

    printf("Recv message type: %x with len: %x and data: %x\n", msg_in.type, msg_in.len, *((uint32_t *)msg_in.data));

    syscall_exit();
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

    create_kernel_tasks();
    
    printf("Create tasks fin\n");
    

    enable_int();
    
    printf("Allowd interrupts\n");
    
    // infiloop();
    sched_yield(); // strange shit.
    return;
}

void infiloop() {
    while (1) {
        printf(MSG_INFINITY);
        halt();
    }  // infiloop
}

void create_kernel_tasks() {
    task_create(0, infiloop, NULL, "ifinity")->status = TASK_RUNNING;
    kernel_tasks_init();
    task_create(0, test1, NULL, "test1")->status = TASK_RUNNING;

    // task_create(0, test1, NULL)->status = TASK_RUNNING;
    // task_create(0, test2, NULL)->status = TASK_RUNNING;
}

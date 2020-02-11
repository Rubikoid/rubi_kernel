#include <kernel/DT/dt.h>
#include <kernel/DT/int.h>
#include <kernel/DT/keyboard.h>
#include <kernel/asm_lib.h>
#include <kernel/defines.h>
#include <kernel/dev/kb_driver.h>
#include <kernel/dev/tty.h>
#include <kernel/kthread/dq.h>
#include <kernel/kthread/ktasks.h>
#include <kernel/memory/heap.h>
#include <kernel/memory/mmu.h>
#include <kernel/messages.h>
#include <kernel/scheduler/task.h>
#include <kernel/serial/serial.h>
#include <kernel/utils/utils.h>
#include <kernel/vfs/elf.h>
#include <kernel/vfs/file.h>
#include <kernel/vga/vga.h>
#include <lib/clist.h>
#include <lib/slist.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>
#include <types.h>

//#if defined(__linux__)
//	#error "This code must be compiled with a cross-compiler"
//#elif !defined(__i386__)
//	#error "This code must be compiled with an x86-elf compiler"
//#endif

#define __MODULE_NAME__ "KERNEL"

typedef struct page_directory_entry_t *pdep_t;
typedef struct page_table_entry_t *ptep_t;

void infiloop();
void create_kernel_tasks();

void test1() {
    klog("Running test\n");
    uint16_t tid = syscall_gettid();
    struct message_t msg_out = {
        .type = 1,
        .len = 2,
        .data = kmalloc(2),
    };
    (*(uint16_t *)msg_out.data) = tid;
    syscall_ksend(ktasks[KERNEL_DQ_TASK_ID]->tid, &msg_out);

    struct message_t msg_in;
    syscall_krecv(&msg_in);

    klog("Recv message type: 0x%x with len: 0x%x and data: 0x%x\n", msg_in.type, msg_in.len, *((uint32_t *)msg_in.data));
    kfree(msg_in.data);
    syscall_exit();
}

void test2() {
    klog("Running test2\n");
    FILE *f = syscall_open(tty_dev_name, FILE_READ);
    uint8_t *test = kmalloc(32);
    syscall_read(f, test, 32);
    klog("Readed msg: %s\n", test);
    kfree(test);
    syscall_exit();
}

void test3() {
    klog("Running test3\n");
    FILE *f = syscall_open(keyboard_dev_name, FILE_READ);
    syscall_ioctl(f, KEYBOARD_IOCTL_ECHO);
    syscall_ioctl(f, IOCTL_FLUSH);
    syscall_exit();
}

void test4() {
    klog("Running test4\n");
    FILE *f = syscall_open(tty_dev_name, FILE_WRITE);
    syscall_ioctl(f, IOCTL_INIT);
    syscall_write(f, "kek1\n", 5);
    syscall_ioctl(f, IOCTL_FLUSH);
    //kfree(test);
    syscall_exit();
}

void test5() {
    klog("Running test5\n");
    char buff[128] = {0};
    char buff2[128] = {0};
    printf("Write something: ");
    scanf("%s %s", &buff, &buff2);
    printf("Readen %s\n", buff);
    printf("Readen %s\n", buff2);
    syscall_exit();
}

void repl() {
    klog("Running repl\n");
    char buff[128] = {0};
    while (TRUE) {
        uprintf("$ ");
        scanf("%s", &buff);
        if (!strcmp(buff, "exit"))
            break;
        else if (!strcmp(buff, "ps"))
            tasks_debug();
        else if (!strcmp(buff, "clear")) {
            if (stdout != NULL) {
                syscall_ioctl(stdout, TTY_IOCTL_CLEAR);
            }
        }
    }
    syscall_exit();
}

void kernel_main(struct multiboot_t *multiboot, void *kstack) {
    init_com(0);
    term_init();
    term_print("Serial and terminal [" G_GREEN "OK" G_WHITE "]\n");

    term_print("MemoryManager");
    init_memory_manager();
    term_print(" [" G_GREEN "OK" G_WHITE "]\n");

    term_print("GDT, IDT, PIC");
    gdt_init();
    idt_init();
    pic_enable();
    term_print(" [" G_GREEN "OK" G_WHITE "]\n");

    disable_int();
    /*
        We should firstly initialize gdt-idt-pic due to possible need in syscalls in driver inits
        And also we disable interrupts, because i hate timer that triggers to early;
    */

    term_print("KB Driver init");
    keyboard_init();
    term_print(" [" G_GREEN "OK" G_WHITE "]\n");

    term_print("TTY Driver init");
    tty_init();
    term_print(" [" G_GREEN "OK" G_WHITE "]\n");

    multiboot = (struct multiboot_t *)(((size_t)multiboot) + 0xC0000000);                       // make virtual ptr to multiboot structure
    multiboot->mods_addr = (struct mod_addr_t *)(((size_t)multiboot->mods_addr) + 0xC0000000);  // make virtual ptr to multiboot structure

    term_print("RubiKernel " KERNEL_VERSION ": Init! [" G_GREEN "OK" G_WHITE "]\n");
    printf("Multiboot: 0x%x; StackStart: 0x%x; Mem_upper: %u\n", multiboot, kstack, multiboot->mem_upper);
    printf("Multiboot modules count: %x\n", multiboot->mods_count);
    printf("Starting addr: %x -> %x\n", multiboot->mods_addr[0].start, multiboot->mods_addr[0].end);

    create_kernel_tasks();
    elf_exec(VIRT(multiboot->mods_addr[0].start));
    enable_int();

    sched_yield();  // strange shit. without first call scheduler going to break
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
    // task_create(0, repl, NULL, "repl")->status = TASK_RUNNING;
    //task_create(0, test1, NULL, "test1")->status = TASK_RUNNING;
    //task_create(0, test2, NULL, "test2")->status = TASK_RUNNING;
    //task_create(0, test3, NULL, "test3")->status = TASK_RUNNING;
    //task_create(0, test4, NULL, "test4")->status = TASK_RUNNING;
    //task_create(0, test5, NULL, "test5")->status = TASK_RUNNING;
    //tasks_debug();
    // task_create(0, test1, NULL)->status = TASK_RUNNING;
    // task_create(0, test2, NULL)->status = TASK_RUNNING;
}

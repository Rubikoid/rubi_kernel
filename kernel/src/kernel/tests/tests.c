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
#include <kernel/tests/tests.h>
#include <kernel/utils/utils.h>
#include <kernel/vfs/elf.h>
#include <kernel/vfs/file.h>
#include <kernel/vfs/fs/initrd.h>
#include <kernel/vga/vga.h>
#include <lib/clist.h>
#include <lib/slist.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>
#include <types.h>

#define __MODULE_NAME__ "TEST"

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

    syscall_exit();
}

void test3() {
    klog("Running test3\n");

    syscall_exit();
}

void test4() {
    klog("Running test4\n");

    syscall_exit();
}

void test5() {
    klog("Running test5\n");

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
            if (stdout != -1) {
                syscall_ioctl(stdout, TTY_IOCTL_CLEAR, TTY_IOCTL_WRITE);
            }
        }
    }
    syscall_exit();
}

void test6() {
    struct dirent_t test;
    struct fs_node_t *root = ((struct fs_node_t *)nodes_list.head->data);
    if (root != NULL) {
        for (int i = 0; i < root->length + 2; i++) {
            int ret = root->readdir(root, i, &test);
            klog("[Test6] %x %x %s\n", ret, test.ino, ret != 0 ? test.name : "---");
        }
    }
    syscall_exit();
}

void test7() {
    struct fs_node_t *in;
    struct fs_node_t *root = ((struct fs_node_t *)nodes_list.head->data);
    if (root != NULL) {
        klog("[Test7] ptr off len flags name\n");
        for (int i = 0; i < root->length + 2; i++) {
            in = root->opennode(root, i);
            if (in != NULL) {
                klog("[Test7] %x %x %x %x %s\n", in, in->inode, in->length, in->flags, in->name);
                if (in->flags & FS_DIRECTORY) {
                    struct fs_node_t *in2;
                    for (int j = 0; j < in->length; j++) {
                        in2 = in->opennode(in, j);
                        klog("[Test7]     %x %x %x %x %s\n", in2, in2->inode, in2->length, in2->flags, in2->name);
                    }
                }
            }
            //int ret = root->readdir(root, i, &test);
            //klog("[Test7] %x %x %s\n", ret, test.ino, test.name == 0 ? "---" : test.name);
        }
    }
    syscall_exit();
}

void test8() {
    {
        struct fs_node_t *x;
        x = resolve_path("/bin/shell");
        klog("%x, %s, %x\n", x, x != NULL ? x->name : "-", x != NULL ? x->length : -1);
    }
    {
        uint32_t fd = -1;
        fd = file_open("/bin/shell", FILE_READ);
        if (fd != -1) {
            uint32_t len = find_file_by_fd(fd)->node->length;
            uint8_t *buff = kmalloc(len);
            // uint8_t buff[];
            int readen = file_read(fd, buff, len);
            if (readen != len) {
                klog("WTF???\n");
            } else {
                elf_exec((struct elf_header_t *)buff);
            }
            kfree(buff);
            // buff[readen] = '\0';
            // klog("%x, %s\n", fd, buff);
        }
    }

    syscall_exit();
}

void test9() {
    tty_ioctl(TTY_IOCTL_READ_MODE_ECHO_ON, TTY_IOCTL_READ);
    char buff[16];
    uint32_t offset = 0;
    int readen = tty_read(buff, &offset, sizeof(buff) - 1);
    buff[readen] = 0;
    klog("r:%x, o:%x, s:%s\n", readen, offset, buff);
    tty_ioctl(TTY_IOCTL_READ_MODE_ECHO_OFF, TTY_IOCTL_READ);
    syscall_exit();
}

void test10() {
    char *buff = "test_10_test\n";
    uint32_t x = 0;
    tty_ioctl(IOCTL_INIT, TTY_IOCTL_WRITE);
    tty_write(buff, &x, strlen(buff) + 1);
    syscall_exit();
}

void test11() {
    fd_t fd1 = -1;
    fd1 = file_open("/dev/TTY", FILE_READ);
    fd_t fd2 = -1;
    fd2 = file_open("/dev/TTY", FILE_WRITE);
    klog("tty check: %x %x\n", fd1, fd2);
    file_ioctl(fd1, IOCTL_INIT, TTY_IOCTL_READ);
    file_ioctl(fd2, IOCTL_INIT, TTY_IOCTL_WRITE);
    file_write(fd2, (uint8_t *)"lolkek\n", 7);
    syscall_exit();
}

void test_empty() {
    syscall_exit();
}

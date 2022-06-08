#include <kernel/dev/tty.h>
#include <kernel/vfs/node.h>
#include <lib/slist.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>
#include <tests_lib.h>

void tests() {
    test_strcmp();
}

void _start() {
    tests();

    printf("Running repl\n");
    char buff[128] = {0};
    struct fs_node_t* pwd = syscall_resolve_path("/");

    while (TRUE) {
        printf("$ ");
        scanf("%s", &buff);
        if (!strcmp(buff, "exit")) {
            syscall_ioctl(stdin, TTY_IOCTL_READ_MODE_ECHO_OFF, 0);
            break;
        } else if (strcmp(buff, "ls") == 0) {
            struct dirent_t* dirent = syscall_malloc(sizeof(struct dirent_t));
            for (int i = 0; i < pwd->length; i++) {
                syscall_readdir(pwd, i, dirent);
                printf("%s\n", dirent->name);
            }
            syscall_free(dirent);
        } else if (!strcmp(buff, "pwd")) {
            printf("%s\n", pwd->name);
        } else if (!strcmp(buff, "ps")) {
            printf("sorry this is not implemented yet in fake-user mode\n");
            // tasks_debug();
        } else if (!strcmp(buff, "clear")) {
            if (stdout != -1) {
                syscall_ioctl(stdout, TTY_IOCTL_CLEAR, TTY_IOCTL_WRITE);  // TTY_IOCTL_CLEAR, TTY_IOCTL_WRITE
            }
        }
    }
    syscall_exit();
}

#include <kernel/dev/tty.h>
#include <lib/slist.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>

void _start() {
    printf("Running repl\n");
    char buff[128] = {0};
    while (TRUE) {
        printf("$ ");
        scanf("%s", &buff);
        if (!strcmp(buff, "exit")) {
            syscall_ioctl(stdin, TTY_IOCTL_READ_MODE_ECHO_OFF, 0);
            break;
        }
        else if (!strcmp(buff, "ps"))
            printf("sorry this is not implemented yet in fake-user mode\n");
        //tasks_debug();
        else if (!strcmp(buff, "clear")) {
            if (stdout != -1) {
                syscall_ioctl(stdout, TTY_IOCTL_CLEAR, TTY_IOCTL_WRITE);  // TTY_IOCTL_CLEAR, TTY_IOCTL_WRITE
            }
        }
    }
    syscall_exit();
}

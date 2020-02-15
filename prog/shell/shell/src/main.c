#include <lib/slist.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>

void _start() {
    // klog("Running repl\n");
    char buff[128] = {0};
    while (TRUE) {
        uprintf("$ ");
        scanf("%s", &buff);
        if (!strcmp(buff, "exit"))
            break;
        else if (!strcmp(buff, "ps")) 
            uprintf("sorry this is not implemented yet in fake-user mode\n");
        //tasks_debug();
        else if (!strcmp(buff, "clear")) {
            /*if (stdout != NULL) {
                syscall_ioctl(stdout, 1 << 8);  // TTY_IOCTL_CLEAR
            }*/
        }
    }
    syscall_exit();
}

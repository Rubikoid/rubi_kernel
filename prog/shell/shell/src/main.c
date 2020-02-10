#include <lib/syscall.h>
#include <lib/slist.h>
#include <lib/stdio.h>
#include <lib/string.h>

void entry_point() {
    // klog("Running repl\n");
    char buff[128] = {0};
    while(TRUE) {
        uprintf("$ ");
        scanf("%s", &buff);
        if(!strcmp(buff, "exit"))
            break; 
        else if(!strcmp(buff, "ps"))
            ;
            //tasks_debug();
        else if(!strcmp(buff, "clear")) {
            if(stdout != NULL) {
                syscall_ioctl(stdout, 1 << 8); // TTY_IOCTL_CLEAR
            }
        }

    }
    syscall_exit();
}

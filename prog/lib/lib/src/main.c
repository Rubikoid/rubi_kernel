#include <lib/slist.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>

void _start() {
    
    syscall_exit();
}

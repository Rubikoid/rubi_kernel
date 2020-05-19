#include <kernel/asm_lib.h>
#include <kernel/dev/tty.h>
#include <kernel/memory/heap.h>
#include <kernel/vfs/file.h>
#include <kernel/vga/vga.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/syscall.h>

fd_t stdin = -1;   // __attribute__ ((section (".data")))
fd_t stdout = -1;  // __attribute__ ((section (".data")))

#ifdef KERNEL

void kvprintf(char *format, va_list arg_list) {
    char ret[256];  // FIXME: possible memory leak
    vsprintf(ret, format, arg_list);
    term_print(ret);
}

void kprintf(char *format, ...) {
    va_list va;
    va_start(va, format);
    kvprintf(format, va);
    va_end(va);
}

void kassert(const char *file, const char *func, uint32_t line, uint8_t expr) {
    if (!expr) {
        kprintf("[" G_RED "ERR" G_WHITE "] Assert in %s at %u (%s)\n", file, line, func);
    }
}

void kdie_assert(const char *file, const char *func, uint32_t line, uint8_t expr) {
    if (!expr) {
        kprintf("[" G_RED "ERR" G_WHITE "] Assert in %s at %u (%s)\n", file, line, func);
        kpanic("assert panic.\n");
    }
}

/*
    Because we can get panic while panic, we should maximize safety of code in panics
*/
void kpanic(char *message, ...) {
    disable_int();
    va_list va;
    va_start(va, message);

    vga_state.term_buffer = (uint16_t *)(0xC0000000 + 0xB8000);
    vga_state.term_col = 0;
    vga_state.term_row = 0;
    vga_state.term_color = V_BLACK << 4 | V_WHITE;
    vga_state.allow_legacy_vga_functions = 1;

    kvprintf(message, va);
    va_end(va);
    halt();
}

#endif

void stdio_init() {
    stdout = syscall_open("/dev/"tty_dev_name, FILE_WRITE);
    syscall_ioctl(stdout, IOCTL_INIT, TTY_IOCTL_WRITE);

    stdin = syscall_open("/dev/"tty_dev_name, FILE_READ);
    syscall_ioctl(stdin, IOCTL_INIT, TTY_IOCTL_READ);
    syscall_ioctl(stdin, TTY_IOCTL_READ_MODE_ECHO_ON, 0);
}

void uvprintf(char *format, va_list arg_list) {
    if (stdout == -1) {
        stdio_init();
    }
    char ret[256];  // FIXME: possible memory leak
    vsprintf(ret, format, arg_list);
    syscall_write(stdout, ret, sizeof(ret));
    syscall_ioctl(stdout, IOCTL_FLUSH, TTY_IOCTL_WRITE);
}

void uprintf(char *format, ...) {
    va_list va;
    va_start(va, format);
    uvprintf(format, va);
    va_end(va);
}

void vscanf(char *format, va_list arg_list) {
    if (stdin == -1) {
        stdio_init();
    }

    uint32_t count = 0, realcount = 0;
    uint32_t readen = 0;
    uint8_t buff[128] = {0};

    for (int i = 0; i < strlen(format); i++) {  // FIXME: crazy solution
        if (format[i] == '%')
            realcount += 1;
        //if(format[i+1] == '%') {
        //    realcount -= 1;
        //    i += 1;
        //}
    }

    while (count < realcount) {
        readen = syscall_read(stdin, buff, sizeof(buff) - 1);
        buff[readen] = '\0';
        count += vsscanf((char *)buff, format, arg_list);
        {  // i hate this crazy solution, but these warnings makes me cry
            size_t _ = 0;
            _ = (size_t)_;
            for (int i = 0; i < count; i++) {
                _ = (size_t)va_arg(arg_list, void *);
            }
        }
    }
}

void scanf(char *format, ...) {
    va_list va;
    va_start(va, format);
    vscanf(format, va);
    va_end(va);
}

// GCC provides these header files automatically
// They give us access to useful things like fixed-width types
#include <stddef.h>
#include <stdint.h>
#include "vga.c"

//#if defined(__linux__)
//	#error "This code must be compiled with a cross-compiler"
//#elif !defined(__i386__)
//	#error "This code must be compiled with an x86-elf compiler"
//#endif

void kernel_main() {
    term_init();

    term_print("Hello, World!\n");
    for (int i = 0; i < 10; i++) {
        char x[2] = "0";
        x[0] += i;
        term_print("Welcome to the kernel");
        term_print(x);
        term_print(".\n");
    }
}

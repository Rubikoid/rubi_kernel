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
    for (int col = 0; col < VGA_COLS; col ++)
	{
		for (int row = 0; row < VGA_ROWS; row ++)
		{
            const size_t index = (VGA_COLS * row) + col;
            vga_buffer[index] = 0;
        }
    }
    return;
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

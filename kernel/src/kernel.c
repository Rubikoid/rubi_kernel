// GCC provides these header files automatically
// They give us access to useful things like fixed-width types
#include <stddef.h>
#include <stdint.h>

#include <kernel/utils/memory.h>
#include <kernel/vga/vga.h>

//#if defined(__linux__)
//	#error "This code must be compiled with a cross-compiler"
//#elif !defined(__i386__)
//	#error "This code must be compiled with an x86-elf compiler"
//#endif

#define DEBUG_ASM asm("xchg %bx, %bx");

extern uint8_t *_gdt;

void kernel_main() {
    term_init();
    init_memory_manager();

    term_print("RubiKernel: Init process [ok]!\n");

    term_print("sizeof(int):");
    term_print_int(sizeof(int), 10);
    term_print("\n");  // 4
    term_print("sizeof(long unsigned int):");
    term_print_int(sizeof(long unsigned int), 10);
    term_print("\n");  // 4
    term_print("sizeof(void *):");
    term_print_int(sizeof(void *), 10);
    term_print("\n");  // 4
    term_print("sizeof(size_t):");
    term_print_int(sizeof(size_t), 10);
    term_print("\n");  // 4

    term_print_int(last_page_ID, 16);
    term_print("\n\n");

    term_print("End!\n");

    return;
}

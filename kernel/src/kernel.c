// GCC provides these header files automatically
// They give us access to useful things like fixed-width types
#include <stddef.h>
#include <stdint.h>

#include <kernel/utils/memory.h>
#include <kernel/utils/utils.h>
#include <kernel/vga/vga.h>

//#if defined(__linux__)
//	#error "This code must be compiled with a cross-compiler"
//#elif !defined(__i386__)
//	#error "This code must be compiled with an x86-elf compiler"
//#endif

extern uint8_t *_gdt;

void kernel_main() {
    term_init();
    init_memory_manager();

    term_print("RubiKernel: Init process [ok]!\n");

    //term_print_int(last_page_ID, 16);
    //term_print("\n\n");

    term_print("End!\n");

    return;
}

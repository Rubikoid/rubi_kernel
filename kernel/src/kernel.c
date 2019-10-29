// GCC provides these header files automatically
// They give us access to useful things like fixed-width types
#include <stddef.h>
#include <stdint.h>

#include <kernel/vga/vga.h>
#include <kernel/utils/memory.h>

//#if defined(__linux__)
//	#error "This code must be compiled with a cross-compiler"
//#elif !defined(__i386__)
//	#error "This code must be compiled with an x86-elf compiler"
//#endif

extern uint8_t* _gdt;

void kernel_main() {
    term_init();
    init_memory_manager();

    term_print("Hello, World!\n");
    for (int i = 0; i < 10; i++) {
        char x[2] = "0";
        x[0] += i;
        term_print("Welcome to the kernel");
        term_print(x);
        term_print(".\n");
    }
    term_print_int(sizeof(int), 10); term_print("\n");                  // 4
    term_print_int(sizeof(unsigned int), 10); term_print("\n");         // 4
    term_print_int(sizeof(long unsigned int), 10); term_print("\n");    // 4
    term_print_int(sizeof(long unsigned long), 10); term_print("\n");   // 8
    term_print_int(sizeof(size_t), 10); term_print("\n");               // 4

    term_print_int(last_page_ID, 16); term_print("\n");

    //term_print_int(boot_page_directory, 16); term_print("\n");
    //term_print_int(boot_page_table, 16); term_print("\n");


    alloc_page();
}

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
    for (int i = 0; i < 3; i++) {
        char x[2] = "0";
        x[0] += i;
        term_print("Welcome to the kernel ");
        term_print(x);
        term_print(".\n");
    }
    term_print("sizeof(int):"); term_print_int(sizeof(int), 10); term_print("\n");                                  // 4
    term_print("sizeof(long unsigned int):"); term_print_int(sizeof(long unsigned int), 10); term_print("\n");      // 4
    term_print("sizeof(void *):"); term_print_int(sizeof(void *), 10); term_print("\n");                            // 4
    term_print("sizeof(size_t):"); term_print_int(sizeof(size_t), 10); term_print("\n");                            // 4

    term_print_int(last_page_ID, 16); term_print("\n\n");

    //term_print_int(boot_page_directory, 16); term_print("\n");
    //term_print_int(boot_page_table, 16); term_print("\n");

    void *a = alloc_page();
    term_print_int(last_page_ID, 16); term_print("\n");
    term_print_int((uint32_t)a, 16); term_print("\n\n");
    void *b = alloc_page();
    term_print_int(last_page_ID, 16); term_print("\n");
    term_print_int((uint32_t)b, 16); term_print("\n\n");

    free_page(b);
    term_print_int(last_page_ID, 16); term_print("\n");
    free_page(a);
    term_print_int(last_page_ID, 16); term_print("\n\n");
}

#include <types.h>

#include <kernel/defines.h>
#include <kernel/utils/dt.h>
#include <kernel/utils/int.h>
#include <kernel/utils/memory.h>
#include <kernel/utils/utils.h>
#include <kernel/vga/vga.h>

//#if defined(__linux__)
//	#error "This code must be compiled with a cross-compiler"
//#elif !defined(__i386__)
//	#error "This code must be compiled with an x86-elf compiler"
//#endif

extern uint8_t* _gdt;

void kernel_main() {
    term_init();
    init_memory_manager();
    
    gdt_init();
    idt_init();

    term_print("[OK] RubiKernel "KERNEL_VERSION": Init!\n");
    printf("Last page id: 0x%x\n", last_page_ID);
    // printf("sizeof(unlong)=%u\n", sizeof(unsigned long));
    // abort("ABORT: test\n");
    //term_print_int(last_page_ID, 16);
    //term_print("\n\n");

    term_print("End!\n");

    return;
}

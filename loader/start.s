.extern kernel_main
.global start

.set MB_ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MB_MEMINFO,  1<<1             /* provide memory map */
.set MB_FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MB_MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set MB_CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

.section .multiboot
	.align 4
	.long MB_MAGIC
	.long MB_FLAGS
	.long MB_CHECKSUM

.section .bss
	.align 16
	stack_bottom:
		.skip 4096 
    stack_top:

.section .text
    start:
        mov $stack_top, %esp
        call kernel_main
        hang:
            cli
            hlt
            jmp hang

ENTRY(kernel_start)
 
SECTIONS
{
	/* . = 1M; */
    . = 0xC0100000; /* 3gb + 1mb */

	_kernel_start = .;
    /* multiboot header*/
    /* .rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.multiboot)
	} */
 
	/* Executable code */
	.text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000)
	{
        *(.multiboot)
		*(.text)
	}
 
	/* Read-only data. */
	.rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xC0000000)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT (ADDR (.data) - 0xC0000000)
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT (ADDR (.bss) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}
    _kernel_end = .;
}


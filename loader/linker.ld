ENTRY(kernel_start)

VIRT_BASE = 0xC0000000;

SECTIONS
{
	. = 1M;

    .boot : ALIGN(4K)
	{
        *(.multiboot)
		*(.boottext)
        . = ALIGN(4k);
	}

    . = . + VIRT_BASE; /* 1mb + header + 3gb */

	_kernel_start = .;
    /* multiboot header*/
    /* .rodata ALIGN(4K) : AT (ADDR (.rodata) - VIRT_BASE)
	{
		*(.multiboot)
	} */
 
	/* Executable code */
	.text ALIGN(4K) : AT (ADDR (.text) - VIRT_BASE)
	{
		*(.text)
	}
 
	/* Read-only data. */
	.rodata ALIGN(4K) : AT (ADDR (.rodata) - VIRT_BASE)
	{
		*(.rodata)
	}
 
	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT (ADDR (.data) - VIRT_BASE)
	{
		*(.data)
	}
 
	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT (ADDR (.bss) - VIRT_BASE)
	{
		*(COMMON)
		*(.bss)
	}
    _kernel_end = .;

    /* Sections to be discarded */
	/DISCARD/ : {
		*(.text.exit)
		*(.data.exit)
		*(.exitcall.exit)
	}

	/* Stabs debugging sections.  */
	.stab 0 : { *(.stab) }
	.stabstr 0 : { *(.stabstr) }
	.stab.excl 0 : { *(.stab.excl) }
	.stab.exclstr 0 : { *(.stab.exclstr) }
	.stab.index 0 : { *(.stab.index) }
	.stab.indexstr 0 : { *(.stab.indexstr) }
	.comment 0 : { *(.comment) }
}


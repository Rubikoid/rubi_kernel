bits 32

extern kernel_main
extern _kernel_start
extern _kernel_end

%define VIRT_BASE 0xC0000000
%define	PHYS(x) ((x) - VIRT_BASE)
%define	VIRT(x) ((x) + VIRT_BASE)

MB_ALIGN    equ  1 << 0                 ; align loaded modules on page boundaries
ME_MINFO    equ  1 << 1                 ; provide memory map
FLAGS       equ  MB_ALIGN | ME_MINFO    ; this is the Multiboot 'flag' field
MAGIC       equ  0x1BADB002             ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum of above, to prove we are multiboot

section .multiboot
	align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM


global _gdt:data
section .bss
	align 16
	stack_bottom:
		resb 16384 
    stack_top:
    
    align 4096
    boot_page_directory:
    	resd 1024
    boot_page_table:
        resd (1024) * 1024
    
    align 16
    gdt: ; one kb for gdt
        resb 1024

section .boottext
    kernel_start:
        xchg bx, bx ; magic break

        mov edi, PHYS(boot_page_table)
        mov eax, 0x003
        mov ecx, VIRT_BASE >> (10 + 2) ; map addr before kernel as is. 2^10 -> 10 because of 1024 PTE per table, 2^2 -> 2 because of 4 bytes per PTE 
        
        .fill_lower_half_with_direct_mappings: ; 
            mov [edi], eax
            add eax, 0x1000 ; it's one in 12's bit, in addr bit of PTE
            add edi, 4
            dec ecx
            jnz .fill_lower_half_with_direct_mappings
        
            mov eax, 0x003
            mov ecx, (1024 * 4) ; map first 16 mb ; one page is 4kb; one table is 4mb
        .fill_higher_half_with_mappings:
            mov [edi], eax
            add eax, 0x1000
            add edi, 4
            dec ecx
            jnz .fill_higher_half_with_mappings

            mov edi, PHYS(boot_page_directory)
            mov edx, PHYS(boot_page_table) + 0x003
            mov ecx, 1024
        .fill_directory_with_table_addrs: 
            mov [edi], edx
            add edx, 0x1000
            add edi, 4
            dec ecx
            jnz .fill_directory_with_table_addrs

        .enable_paging:

            mov ecx, PHYS(boot_page_directory)
            mov cr3, ecx

            mov ecx, cr0
            or ecx, 0x80010000
            mov cr0, ecx

        .paging_enabled_jump_naxyi:
            xchg bx, bx ; magic break
            mov eax, higher_kernel_start
            jmp eax


global kernel_start:function
section .text
    higher_kernel_start: ; kernel
        mov edi, boot_page_directory
        mov ecx, (VIRT_BASE) / (4 * 1024 * 1024) ; 3gb by 4mb (get count of first 3gb pages)

        .k4:
            mov [edi], dword 0
            add edi, 4
            dec ecx
            jnz .k4
        
        mov esp, stack_top
        xchg bx, bx ; magic break
        call kernel_main
        .hang:
            cli
            hlt
            jmp .hang

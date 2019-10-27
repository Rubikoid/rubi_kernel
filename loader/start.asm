bits 32

extern kernel_main
global kernel_start

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

section .boottext
    kernel_start:
        xchg bx, bx ; magic break

        lea edi, [PHYS(boot_page_table)] ; phys addr of kernel
        mov eax, 0x003
        mov ecx, 1024 * 1024;[VIRT_BASE >> 12]

        .fill_tables_with_direct_mappings: ; 
            mov [edi], eax
            add eax, 0x1000 ; it's one in 12's bit, in addr bit of PTE
            add edi, 4
            dec ecx
            jnz .fill_tables_with_direct_mappings


            lea edi, [PHYS(boot_page_directory)]
            lea edx, [PHYS(boot_page_table) + 0x003]
            mov ecx, 1024
        .fill_directory_with_table_addrs: 
            mov [edi], edx
            add edx, 0x1000
            add edi, 4
            dec ecx
            jnz .fill_directory_with_table_addrs

        .enable_paging:
            xchg bx, bx ; magic break

            lea ecx, [PHYS(boot_page_directory)]
            mov cr3, ecx

            mov ecx, cr0
            or ecx, 0x80010000
            mov cr0, ecx

        .paging_enabled_jump_naxyi:
            xchg bx, bx ; magic break
            mov eax, higher_kernel_start
            jmp eax

section .text
    higher_kernel_start:
        lea edi, [PHYS(boot_page_directory)]
        mov ecx, VIRT_BASE >> 12 >> 10

        .k4:
            mov [edi], dword 0
            add edi, 4
            dec ecx
            jnz .k4
        
        
        mov esp, stack_top
        call kernel_main
        .hang:
            cli
            hlt
            jmp .hang

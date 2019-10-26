bits 32

extern kernel_main
global kernel_start

extern _kernel_start
extern _kernel_end

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
    	resb 4096
    boot_page_table1:
        resb 4096


section .text
    kernel_start:
        mov edi, [boot_page_table1 - 0xC0000000] ; phys addr of kernel
        mov esi, 0 ; first addr to map is 0
        mov ecx, 1023 ; map 1023 pages, 1024 - vga buffer

        .k1: ; TODO: разобраться, как эта херня работает
            cmp esi, [_kernel_start - 0xC0000000]
            jl .k2
            cmp esi, [_kernel_end - 0xC0000000]
            jge .k3

            mov edx, esi
            or edx, 0x003
            mov edi, edx
        
        .k2:
            add esi, 4096
            add edi, 4
            loop .k1

        .k3:
            mov eax, 0x000B8000 | 0x003
            mov [boot_page_table1 - 0xC0000000 + 1023 * 4], eax

            lea eax, [boot_page_table1 - 0xC0000000 + 0x003]
            mov [boot_page_directory - 0xC0000000 + 0], eax
            mov [boot_page_directory - 0xC0000000 + 768 * 4], eax

            lea ecx, [boot_page_directory - 0xC0000000]
            mov cr3, ecx

            mov ecx, cr0
            or ecx, 0x80010000
            mov cr0, ecx

            jmp .most_init

    .most_init:
        mov eax, 0
        mov [boot_page_directory + 0], eax
        mov ecx, cr3
        mov cr3, ecx
        
        mov esp, stack_top
        call kernel_main
        .hang:
            cli
            hlt
            jmp .hang

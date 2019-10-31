bits 32

extern kernel_main
extern _kernel_start
extern _kernel_end

; one page is 4kb; one table is 4mb
%define PAGE_SIZE 0x1000
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


global boot_page_directory:data, boot_page_table:data
section .bss
	align 16
	stack_bottom:
		resb 16384*2
    stack_top:
    
    align 4096
    boot_page_directory:
    	resd 1024
    boot_page_table:
        resd (1024) * 1024

section .boottext
    kernel_start:
        ; xchg bx, bx ; magic break

        mov edi, PHYS(boot_page_table)
        mov eax, 0x003
        mov ecx, VIRT_BASE >> (10 + 2) ; map addr before kernel as is. 2^10 -> 10 because of 1024 PTE per table, 2^2 -> 2 because of 4 bytes per PTE 
        
        .fill_lower_half_with_direct_mappings:
            mov [edi], eax
            add eax, 0x1000 ; it's one in 12's bit, in addr bit of PTE
            add edi, 4
            dec ecx
            jnz .fill_lower_half_with_direct_mappings
        
            ; fill_higher_half_with_mappings
            mov eax, 0x003
            mov ecx, (1024 * 4) ; map first 16 mb ; one page is 4kb; one table is 4mb
        .fill_higher_half_with_mappings:
            mov [edi], eax
            add eax, 0x1000
            add edi, 4
            dec ecx
            jnz .fill_higher_half_with_mappings

            ; fill_directory_with_table_addrs
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
            ; xchg bx, bx ; magic break
            mov eax, higher_kernel_start
            jmp eax


global kernel_start, disable_int, enable_int, out_byte, in_byte, gdt_load, idt_load, halt
section .text
    ;push    ebp
    ;mov     ebp, esp

    ;mov     esp, ebp
    ;pop     ebp
    ;ret

    ;gdt_load(void *gdt_ptr); Load gdt
    gdt_load:
        mov eax, [esp+4]
        lgdt [eax]
        jmp   0x08:.reload_segment
    .reload_segment:
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        ret

    ;idt_load(unsigned long *addr); Load gdt
    idt_load:
        mov eax, [esp+4]
        lidt [eax]
        ret

    ;void disable_int(); Disable interruptions
    disable_int:
        cli
        ret
    
    ;void enable_int(); Enable interruptions
    enable_int:
        sti
        ret

    ;void halt(); Halt!
    halt:
        hlt
        ret

    ;void out_byte(u16 port, u8 value);    
    out_byte:
            mov     edx, [esp + 4]      ; port
            mov     al, [esp + 4 + 4]   ; value
            out     dx, al
            nop             ;
            nop
            ret

    ;u8 in_byte(u16 port);
    in_byte:
            mov     edx, [esp + 4]      ; port
            xor     eax, eax
            in      al, dx
            nop                         ;
            nop
            ret

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

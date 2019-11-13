bits 32

global reload_kernel_segments, disable_int, enable_int, outb, inb, gdt_load, idt_load, halt
section .text
;push    ebp
    ;mov     ebp, esp

    ;mov     esp, ebp
    ;pop     ebp
    ;ret

    ;reload_kernel_segments(void); Load kernel segments data
    reload_kernel_segments:
    jmp   0x08:.reload_segment
    .reload_segment:
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        ret

    reload_user_segments:
    jmp   0x08:.reload_segment
    .reload_segment:
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        ret

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

    ;void outb(u16 port, u8 value);    
    outb:
            mov     edx, [esp + 4]      ; port
            mov     al, [esp + 4 + 4]   ; value
            out     dx, al
            nop             ;
            nop
            ret

    ;u8 inb(u16 port);
    inb:
            mov     edx, [esp + 4]      ; port
            xor     eax, eax
            in      al, dx
            nop                         ;
            nop
            ret

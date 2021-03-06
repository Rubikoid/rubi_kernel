bits 32

global ddd, switch_kcontext, enable_paging, get_cr3, get_eflags, reload_kernel_segments, disable_int, enable_int, gdt_load, idt_load, halt
global outb, inb, outw, inw, outdw, indw
section .text
    ;push    ebp
    ;mov     ebp, esp

    ;mov     esp, ebp
    ;pop     ebp
    ;ret

    ddd:
        xchg bx, bx
        mov eax, 1
        int 80h
        ret

    ;void switch_kcontext(uint32_t esp, uint32_t cr3)
    switch_kcontext:
        mov ebp, [esp + 4] ; esp
        mov eax, [esp + 8] ; cr3
        mov cr3, eax
        mov eax, cr0
        or eax, 0x80000001
        mov cr0, eax
        mov esp, ebp
        popad
        iret
        ; ret

    ;void enable_paging(void *page_directory)
    enable_paging:
        mov eax, [esp + 4]
        mov cr3, eax
        mov eax, cr0
        or eax, 0x80000001
        mov cr0, eax
        ret

    ;void *get_cr3(void)
    get_cr3:
        mov eax, cr3
        ret
    
    ;uint32_t get_eflags(void)
    get_eflags:
        pushfd
        pop eax
        ret

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

    ;void gdt_load(void *gdt_ptr); Load gdt
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

    ;void idt_load(unsigned long *addr); Load gdt
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


    ;void outw(u16 port, u16 value);    
    outw:
        mov     edx, [esp + 4]      ; port
        mov     ax, [esp + 4 + 4]   ; value
        out     dx, ax
        nop             ;
        nop
        ret

    ;u16 inw(u16 port);
    inw:
        mov     edx, [esp + 4]      ; port
        xor     eax, eax
        in      ax, dx
        nop                         ;
        nop
        ret


    ;void outdw(u32 port, u16 value);    
    outdw:
        mov     edx, [esp + 4]      ; port
        mov     eax, [esp + 4 + 4]   ; value
        out     dx, eax
        nop             ;
        nop
        ret

    ;u32 indw(u16 port);
    indw:
        mov     edx, [esp + 4]      ; port
        xor     eax, eax
        in      eax, dx
        nop                         ;
        nop
        ret

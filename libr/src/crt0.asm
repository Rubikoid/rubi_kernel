bits 32

extern main
global _start

section .text
    _start:
        call main

        ; move exit code to ebx. If it exists...
        mov ebx, eax
        
        ; call exit
        mov eax, 0x1
        int 80h
        ret

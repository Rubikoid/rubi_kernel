bits 32

global fsyscall
section .text
    ; uint32_t fsyscall(uint32_t id, uint32_t arg1, uint32_t arg2, uint32_t arg3)
    fsyscall:
        mov eax, [esp + 4]
        mov ebx, [esp + 8]
        mov ecx, [esp + 12]
        mov edx, [esp + 16]
        int 80h
        ret
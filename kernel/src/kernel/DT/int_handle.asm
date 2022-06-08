bits 32

section .text

extern reload_kernel_segments

extern cint_division_by_zero, cint_double_fail, cint_segment_not_present, cint_general_protect, cint_aligment_check, cint_page_fault, cint_syscall, cint_serial
global int_division_by_zero, int_double_fail, int_segment_not_present, int_general_protect, int_aligment_check, int_page_fault, int_syscall, int_serial

extern cint_timer, cint_keyboard
global int_timer, int_keyboard

%macro handler 1
%1:
    cli
	pushad
	cld
	call c%1
	popad
    sti
	iret	
%endmacro

%macro handler1 1
%1:
    pushad
	cld
	call c%1
	popad
	iret	
%endmacro

handler int_division_by_zero
handler int_double_fail
handler int_segment_not_present
handler int_aligment_check
handler1 int_keyboard
handler1 int_serial

int_general_protect:
    cli
	pushad
	cld
	mov eax, cs
	push eax
	call cint_general_protect
	pop eax
	popad
    sti
	iret

int_page_fault:
    cli
	pushad
	mov eax, cr2
	push eax
	cld
	call cint_page_fault
	pop eax
	popad
    sti
	iret
	
int_syscall:
    pushad
    cld
    call cint_syscall
    add esp, 32 ; we don't need to restore poped args
    ; popad
    iret

int_timer:
    cli ; i belive we should disallow interrupts?
	pushad
	cld
    mov ebp, esp
    mov ebx, ebp
    push ebx ; &reg
    ; EAX (+0), ECX (+4), EDX (+8), EBX (+12), ESP (+16), EBP (+20), ESI (+24), EDI (+28), = 8 * 4 = 32 bytes before EIP
    add ebx, 32
    push ebx ; & ret addr
	call cint_timer
    mov esp, ebp
	popad
    sti ; i belive we should allow interrupts?
	iret

;:
;	pushad
;	cld
;	call 
;	popad
;	iret

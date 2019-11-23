bits 32

section .text

extern reload_kernel_segments

extern cint_division_by_zero, cint_double_fail, cint_segment_not_present, cint_general_protect, cint_aligment_check, cint_page_fault, cint_syscall
global int_division_by_zero, int_double_fail, int_segment_not_present, int_general_protect, int_aligment_check, int_page_fault, int_syscall

extern cint_timer, cint_keyboard
global int_timer, int_keyboard

int_division_by_zero:
    cli
	pushad
	cld
	call cint_division_by_zero
	popad
    sti
	iret	

int_double_fail:
    cli
	pushad
	cld
	call cint_double_fail
	popad
    sti
	iret

int_segment_not_present:
    cli
	pushad
	cld
	call cint_segment_not_present
	popad
    sti
	iret

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

int_aligment_check:
    cli
	pushad
	cld
	call cint_aligment_check
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
    popad
    iret

int_keyboard:
	pushad
	cld
	call cint_keyboard
	popad
	iret

int_timer:
    ; cli
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
    ; sti
	iret

;:
;	pushad
;	cld
;	call 
;	popad
;	iret

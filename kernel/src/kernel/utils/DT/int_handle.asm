bits 32

section .text

extern reload_kernel_segments

extern cint_division_by_zero, cint_double_fail, cint_segment_not_present, cint_general_protect, cint_aligment_check, cint_page_fault
global int_division_by_zero, int_double_fail, int_segment_not_present, int_general_protect, int_aligment_check, int_page_fault

extern cint_timer, cint_keyboard
global int_timer, int_keyboard

int_division_by_zero:
	pushad
	cld
	call cint_division_by_zero
	popad
	iret	

int_double_fail:
	pushad
	cld
	call cint_double_fail
	popad
	iret

int_segment_not_present:
	pushad
	cld
	call cint_segment_not_present
	popad
	iret

int_general_protect:
	pushad
	cld
	mov eax, cs
	push eax
	call cint_general_protect
	pop eax
	popad
	iret

int_aligment_check:
	pushad
	cld
	call cint_aligment_check
	popad
	iret

int_page_fault:
	pushad
	mov eax, cr2
	push eax
	cld
	call cint_page_fault
	pop eax
	popad
	iret
	
int_keyboard:
	pushad
	cld
	call cint_keyboard
	popad
	iret

int_timer:
	pushad
	cld
	call cint_timer
	popad
	iret

;:
;	pushad
;	cld
;	call 
;	popad
;	iret

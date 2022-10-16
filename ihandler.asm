%include "common.asm"

global handler_entry_table
global SysCallHanderEntry

extern handler_table
extern SysCallHandler
extern gCurrentTaskAddr

[section .text]
[bits 32]
SysCallHanderEntry:
    sub esp, 4

	pushad

	push ds
	push es
	push fs
	push gs

	mov dword [gCurrentTaskAddr], esp

	mov si, Video32Selector
	mov gs, si

	mov si, FlatModeDataSelector
	mov ds, si
	mov es, si
	mov fs, si

	mov ss, si
	mov esp, BaseOfLoader
	
	sti

	push edx
	push ecx
	push ebx
	push eax

	call SysCallHandler

	; save system call return value
	mov ebx, dword [gCurrentTaskAddr]
	mov dword [ebx + 0x2c], eax

	mov esp, dword [gCurrentTaskAddr]

	pop gs
	pop fs
	pop es
	pop ds

	popad

	add esp, 4

	iret	
; end of [section .text]

[section .data]
[bits 32]
handler_entry_table:

 ; end of [section .data] 

%macro INTERRUPT_HANDLER 2
[section .text]
[bits 32]
interrupt_%1_handler:

%ifn %2
    push 0
%endif

interrupt_%1_entry:
	pushad

	push ds
	push es
	push fs
	push gs

	mov ecx, dword [esp + 4 * 12]

CheckRPL_%1:
	mov ax, gs
	cmp ax, 0
	jne RPL0_%1

RPL3_%1:
	mov dword [gCurrentTaskAddr], esp

	mov ax, Video32Selector
	mov gs, ax

	mov ax, FlatModeDataSelector
	mov ds, ax
	mov es, ax
	mov fs, ax

	mov ss, ax
	mov esp, BaseOfLoader

	push 3

	jmp next_%1

RPL0_%1:
	push 0

next_%1:	
	sti

	push ecx
	push %1

	mov eax, dword [handler_table + %1 * 4]

	cmp eax, 0
	je no_call_%1

	call eax

no_call_%1:
	add esp, 12

	cmp dword [esp - 4], 0
	je iret_%1

	mov esp, dword [gCurrentTaskAddr]

iret_%1:
	pop gs
	pop fs
	pop es
	pop ds

	popad

	add esp, 4

	iret
; end of [section .text]

[section .data]
[bits 32]
	dd	interrupt_%1_handler

%endmacro

; define interrupt handler function
INTERRUPT_HANDLER 0x00, 0; divide by zero
INTERRUPT_HANDLER 0x01, 0; debug
INTERRUPT_HANDLER 0x02, 0; non maskable interrupt
INTERRUPT_HANDLER 0x03, 0; breakpoint

INTERRUPT_HANDLER 0x04, 0; overflow
INTERRUPT_HANDLER 0x05, 0; bound range exceeded
INTERRUPT_HANDLER 0x06, 0; invalid opcode
INTERRUPT_HANDLER 0x07, 0; device not avilable

INTERRUPT_HANDLER 0x08, 1; double fault
INTERRUPT_HANDLER 0x09, 0; coprocessor segment overrun
INTERRUPT_HANDLER 0x0a, 1; invalid TSS
INTERRUPT_HANDLER 0x0b, 1; segment not present

INTERRUPT_HANDLER 0x0c, 1; stack segment fault
INTERRUPT_HANDLER 0x0d, 1; general protection fault
INTERRUPT_HANDLER 0x0e, 1; page fault
INTERRUPT_HANDLER 0x0f, 0; reserved

INTERRUPT_HANDLER 0x10, 0; x87 floating point exception
INTERRUPT_HANDLER 0x11, 1; alignment check
INTERRUPT_HANDLER 0x12, 0; machine check
INTERRUPT_HANDLER 0x13, 0; SIMD Floating - Point Exception

INTERRUPT_HANDLER 0x14, 0; Virtualization Exception
INTERRUPT_HANDLER 0x15, 1; Control Protection Exception
INTERRUPT_HANDLER 0x16, 0; reserved
INTERRUPT_HANDLER 0x17, 0; reserved

INTERRUPT_HANDLER 0x18, 0; reserved
INTERRUPT_HANDLER 0x19, 0; reserved
INTERRUPT_HANDLER 0x1a, 0; reserved
INTERRUPT_HANDLER 0x1b, 0; reserved

INTERRUPT_HANDLER 0x1c, 0; reserved
INTERRUPT_HANDLER 0x1d, 0; reserved
INTERRUPT_HANDLER 0x1e, 0; reserved
INTERRUPT_HANDLER 0x1f, 0; reserved

INTERRUPT_HANDLER 0x20, 0; clock 时钟中断
INTERRUPT_HANDLER 0x21, 0
INTERRUPT_HANDLER 0x22, 0
INTERRUPT_HANDLER 0x23, 0
INTERRUPT_HANDLER 0x24, 0
INTERRUPT_HANDLER 0x25, 0
INTERRUPT_HANDLER 0x26, 0
INTERRUPT_HANDLER 0x27, 0
INTERRUPT_HANDLER 0x28, 0; rtc 实时时钟
INTERRUPT_HANDLER 0x29, 0
INTERRUPT_HANDLER 0x2a, 0
INTERRUPT_HANDLER 0x2b, 0
INTERRUPT_HANDLER 0x2c, 0
INTERRUPT_HANDLER 0x2d, 0
INTERRUPT_HANDLER 0x2e, 0
INTERRUPT_HANDLER 0x2f, 0
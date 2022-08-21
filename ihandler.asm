%include "common.asm"

global DefaultHandlerEntry
global TimerHandlerEntry
global DebugHandlerEntry
global SysCallHandlerEntry

extern TimerHandler
extern DebugHandler
extern SysCallHandler
extern gCurrentTaskAddr

[section .handler]
[bits 32]

; fault service routine
%macro BeginFSR 0
	cli 

	pushad

	push ds
	push es
	push fs
	push gs

	mov si, Video32Selector
	mov gs, si

	mov si, FlatModeDataSelector
	mov ds, si
	mov es, si
	mov fs, si

	mov ss, si
	mov esp, BaseOfBoot
%endmacro

; interrupt service routine
%macro BeginISR 0
	cli

	sub esp, 4

	pushad

	push ds
	push es
	push fs
	push gs

	mov si, Video32Selector
	mov gs, si

	mov si, FlatModeDataSelector
	mov ds, si
	mov es, si
	mov fs, si

	mov ss, si
	mov esp, BaseOfBoot
%endmacro

%macro EndISR 0

	mov esp, [gCurrentTaskAddr]

	pop gs
	pop fs
	pop es
	pop ds

	popad

	add esp, 4

%endmacro

; Default interrupt hanlder entry
DefaultHandlerEntry:
BeginISR

EndISR
	iret

; Time interrupt hanlder entry
TimerHandlerEntry:
BeginISR

	call TimerHandler

EndISR
	iret

; Debug interrupt hanlder entry
DebugHandlerEntry:
BeginISR

	push dword [gCurrentTaskAddr]

	call DebugHandler

	add esp, 0x04

EndISR
	iret

; System Call interrupt hanlder entry
SysCallHandlerEntry:
BeginISR

	push edx
	push ecx
	push ebx
	push eax

	call SysCallHandler

	add esp, 0x10

	; save system call return value
	mov ebx, dword [gCurrentTaskAddr]
	
	mov dword [ebx + 0x2c], eax

EndISR
	iret		

; end of [section .handler]
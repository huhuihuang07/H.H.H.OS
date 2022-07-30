%include "common.asm"

global DefaultHandlerEntry
global TimerHandlerEntry
global DebugHandlerEntry

extern TimerHandler
extern DebugHandler
extern gCurrentTaskAddr

[section .handler]
[bits 32]

%macro BeginISR 0

	sub esp, 4

	pushad

	push ds
	push es
	push fs
	push gs

	mov ax, Video32Selector
	mov gs, ax

	mov ax, FlatModeDataSelector
	mov ds, ax
	mov es, ax
	mov fs, ax

	mov ss, ax
	mov esp, BaseOfBoot

	; sti
%endmacro

%macro EndISR 0
	; cli

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

; end of [section .handler]
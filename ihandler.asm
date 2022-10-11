%include "common.asm"

global DefaultInterruptHandlerEntry
global DefaultFaultHandlerEntry
global TimerHandlerEntry
global DebugHandlerEntry
global SysCallHandlerEntry
global PageFaultHandlerEntry

extern DefaultInterruptHandler
extern DefaultFaultHandler
extern TimerHandler
extern DebugHandler
extern SysCallHandler
extern PageFaultHandler

extern gCurrentTaskAddr
extern SendEOI

[section .handler]
[bits 32]

%macro CheckRPL 0
	push ax
	mov ax, gs
	cmp ax, 0
	pop ax
%endmacro

; fault service routine RPL = 0
%macro BeginFSRRPL0 0
	sti

	pushad

	push ds
	push es
	push fs
	push gs
%endmacro

; fault service routine RPL = 3
%macro BeginFSRRPL3 0
	sti 

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
%endmacro

; interrupt service routine RPL = 0
%macro BeginISRRPL0 0
	sti

	pushad

	push ds
	push es
	push fs
	push gs
%endmacro

; interrupt service routine RPL = 3
%macro BeginISRRPL3 0
	sti

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
%endmacro

%macro EndISRRPL0 0
	pop gs
	pop fs
	pop es
	pop ds

	popad

	retf
%endmacro

%macro EndISRRPL3 0

	mov esp, dword [gCurrentTaskAddr]

	pop gs
	pop fs
	pop es
	pop ds

	popad

	add esp, 4

	iret
%endmacro

%macro EndFSRRPL0 0
	pop gs
	pop fs
	pop es
	pop ds

	popad

	add esp, 4

	retf
%endmacro	

; Default fault hanlder entry
DefaultFaultHandlerEntry:
CheckRPL
	je DefaultFaultRPL3

BeginFSRRPL0
	mov eax, dword [esp + 12 * 4]

	push eax

	call DefaultFaultHandler

	add esp, 4
EndFSRRPL0

DefaultFaultRPL3:
BeginFSRRPL3
	mov ebx, dword [gCurrentTaskAddr]

	push dword [ebx + 12 * 4]

	call DefaultFaultHandler

	add esp, 0x04
EndISRRPL3	


; Default interrupt hanlder entry
DefaultInterruptHandlerEntry:
CheckRPL
	je DefaultInterruptRPL3

BeginISRRPL0
	call DefaultInterruptHandler
EndISRRPL0

DefaultInterruptRPL3:
BeginISRRPL3
	call DefaultInterruptHandler
EndISRRPL3

; Time interrupt hanlder entry
TimerHandlerEntry:
CheckRPL
	je TimerRPL3

BeginISRRPL0
	push 0x20

	call SendEOI

	add esp, 4
EndISRRPL0

TimerRPL3:
BeginISRRPL3
	call TimerHandler
EndISRRPL3

; Debug interrupt hanlder entry
DebugHandlerEntry:
CheckRPL
	je DebugRPL3

BeginISRRPL0
EndISRRPL0

DebugRPL3:
BeginISRRPL3
	push dword [gCurrentTaskAddr]

	call DebugHandler

	add esp, 0x04
EndISRRPL3

; System Call interrupt hanlder entry
SysCallHandlerEntry:
CheckRPL
	je SysCallRPL3

BeginISRRPL0
EndISRRPL0

SysCallRPL3:

BeginISRRPL3
	push edx
	push ecx
	push ebx
	push eax

	call SysCallHandler

	add esp, 0x10

	; save system call return value
	mov ebx, dword [gCurrentTaskAddr]
	
	mov dword [ebx + 0x2c], eax
EndISRRPL3

; Page Fault handler entry
PageFaultHandlerEntry:
CheckRPL
	je PageFaultRPL3	

BeginFSRRPL0
	mov eax, dword [esp + 12 * 4]

	push eax

	call PageFaultHandler

	add esp, 4
EndFSRRPL0

PageFaultRPL3:
BeginFSRRPL3
	mov ebx, dword [gCurrentTaskAddr]

	push dword [ebx + 12 * 4]

	call PageFaultHandler

	add esp, 0x04
EndISRRPL3	

; end of [section .handler]
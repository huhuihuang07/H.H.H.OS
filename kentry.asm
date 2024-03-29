%include "common.asm"

global _start

extern gGdtInfo
extern gIdtInfo
extern gMemInfo
extern gMemSize

extern KMain

[section .text]
[bits 32]
_start:

	call InitGlobal

	mov eax, HellStr
	push eax

	mov eax, InfoStr
	push eax

	push esp

	mov eax, 0x02
	push eax

	call KMain

	add esp, 0x10

last:
	hlt
	jmp last

InitGlobal:
	push ebp
	mov ebp, esp

	push eax
	push ebx
	push ecx

	push ds
	push es

	mov ax, FlatModeDataSelector
	mov ds, ax

	mov ax, KernelDataSelector
	mov es, ax

	; init GDT Info
	mov eax, dword [es:2]
	mov dword [ds:gGdtInfo + 0], eax

	mov ax, word [es:0]
	inc ax
	shr ax, 3
	mov word [ds:gGdtInfo + 4], ax

	; init IDT Info
	mov eax, dword [es:8]
	mov dword [ds:gIdtInfo + 0], eax

	mov ax, word [es:6]
	inc ax
	shr ax, 3
	mov word [ds:gIdtInfo + 4], ax

	; init Memory size
	mov eax, dword [es:12]
	mov dword [ds:gMemSize + 0], eax

	; init Memory Info
	mov ecx, 20
	xor ebx, ebx

	mov al, byte [es:bx + 16]
	mov byte [ds:ebx + gMemInfo + 0], al

readMemInfo:
	; BaseAddrLow
	mov eax, dword [es:bx + 17]
	mov dword [ds:ebx + gMemInfo + 1], eax

	; BaseAddrHigh
	mov eax, dword [es:bx + 21]
	mov dword [ds:ebx + gMemInfo + 5], eax

	; LengthLow
	mov eax, dword [es:bx + 25]
	mov dword [ds:ebx + gMemInfo + 9], eax

	; LengthHigh
	mov eax, dword [es:bx + 29]
	mov dword [ds:ebx + gMemInfo + 13], eax

	; Type
	mov eax, dword [es:bx + 33]
	mov dword [ds:ebx + gMemInfo + 17], eax

	add ebx, 0x14
	loop readMemInfo

	pop es
	pop ds

	pop ecx
	pop ebx
	pop eax

	leave
	ret	

[section .data]
[bits 32]	
	InfoStr  db    "H.H.H.OS version : 0.11", 0
	HellStr  db    "Hello world!", 0
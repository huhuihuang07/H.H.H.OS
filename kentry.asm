global _start

extern KMain
extern ClearScreen

[section .text]
[bits 32]
_start:

	call ClearScreen

	mov eax, HellStr
	push eax

	mov eax, InfoStr
	push eax

	push esp

	mov eax, 0x02
	push eax

	call KMain

	add esp, 0x0f

last:
	hlt
	jmp last

[section .data]
[bits 32]	
	InfoStr  db    "H.H.H.OS version : 0.11", 0
	HellStr  db    "Hello world!", 0
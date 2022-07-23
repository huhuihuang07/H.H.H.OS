%include "common.asm"

global printChar

[section .text]
[bits 32]

printChar:
	push ebp
	mov ebp, esp

	push eax
	push edi

	push gs

	mov ax, Video32Selector
	mov gs, ax

	mov edi, dword [ebp +  8]
	shl edi, 1

	mov eax, dword [ebp + 12]

	mov word [gs:edi], ax

	pop gs

	pop edi
	pop eax

	mov eax, 1

	leave
	ret

; end of [section .text]
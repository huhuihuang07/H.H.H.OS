global RunTask

[section .text]
[bits 32]

; Running task
RunTask:
	push ebp
	mov ebp, esp

	mov esp, [ebp + 8]

	lldt word [esp + 200]
	ltr word [esp + 202]

	pop gs
	pop fs
	pop es
	pop ds

	popad

	add esp, 4

	iret

; end of [section .text]
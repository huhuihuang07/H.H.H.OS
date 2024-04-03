%include "blfunc.asm"
%include "common.asm"

org BaseOfBoot

interface:
	BaseOfStack equ BaseOfBoot
	LoadAddress equ BaseOfLoader

	TargetStr db  "LOAD    BIN"
	TargetLen equ $ - TargetStr

BLMain:                             
	xor ax, ax
	mov ds,	ax
	mov es, ax
	mov ss, ax

	mov sp, BaseOfStack

	sub sp, DIR_Length
	push sp

	mov ax, LoadAddress
	push ax

	mov ax, TargetLen
	push ax

	mov ax, TargetStr
	push ax

	mov ax, Buffer
	push ax

	call loadTarget

	add sp, 0x0a

	cmp dx, 0
	jnz Enter

	mov bp, FindErrorStr
	mov cx, FindErrorLen
	jmp print

Enter:
	mov eax, LoadAddress
	add eax, dword [BaseOfStack - DIR_Length + DIR_FileSize]
	cmp eax, BaseOfKernel
	jna Next

	mov bp, LenErrorStr
	mov cx, LenErrorLen
	jmp print
	
Next:
	jmp 0:LoadAddress

DefineStr:
	FindErrorStr db "find error"
	FindErrorLen equ $ - FindErrorStr

	LenErrorStr db "len error"
	LenErrorLen equ $ - LenErrorStr

Buffer:
	times 510 - ($ -$$) db 0x00

END:                                   ; 系统引导标识
	db 0x55, 0xaa
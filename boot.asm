%include "blfunc.asm"
%include "common.asm"

org BaseOfBoot

interface:
	BaseOfStack equ BaseOfBoot - DIR_Length
	LoadAddress equ BaseOfLoader

	TargetStr db  "LOAD    BIN"
	TargetLen equ $ - TargetStr

BLMain:                             
	mov ax, cs
	mov ds,	ax
	mov es, ax
	mov ss, ax

	mov sp, BaseOfStack

	call loadTarget

	cmp dx, 0
	jnz Enter

	mov bp, FindErrorStr
	mov cx, FindErrorLen
	jmp print

Enter:
	mov eax, LoadAddress
	add eax, dword [BaseOfStack + DIR_FileSize]
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
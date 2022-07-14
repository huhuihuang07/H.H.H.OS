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
	jz Error

	jmp 0:LoadAddress
	
DefineStr:
	ErrorStr db "Not find loader ..."
	ErrorLen equ $ - ErrorStr

Buffer:
	times 510 - ($ -$$) db 0x00

END:                                   ; 系统引导标识
	db 0x55, 0xaa
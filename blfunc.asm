define:
	FAT1Address     equ 0x7e00 ; FAT表载入内存地址
	
	RootEntryOffset equ 19 ; 根目录起始扇区
	RootEntryLength equ 14 ; 根目录扇区总数

	FatTableOffset equ 1 ; FAT表起始扇区
	FatTableLength equ 9 ; FAT表扇区总数

	DIR_Name        equ 0x00 ; 文件名8字节，扩展名3字节
	DIR_Attr        equ 0x0b ; 文件属性
	Reserve         equ 0x0c ; 保留位
	DIR_WrtTime     equ 0x16 ; 最后一次写入时间
	DIR_WrtDate     equ 0x18 ; 最后一次写入日期
	DIR_FstClus     equ 0x1a ; 文件开始簇号
	DIR_FileSize    equ 0x1c ; 文件大小
	DIR_Length      equ 0x20 ; 目录项长度

; FAT12 header
BS_JmpBoot:    jmp short BOOT_CODE 
nop                                    ; 跳转指令（3 Byte）
BS_OEMName     db "MSWIN4.1" 	   	   ; OEM字符串，必须为8个字符，不足以空格填空
BPB_BytsPerSec dw 0x0200               ; 每扇区字节数（512 Byte）
BPB_SecPerClus db 0x01             	   ; 每簇占用的扇区数
BPB_RsvdSecCnt dw 0x0001		       ; Boot占用的扇区数
BPB_NumFATs    db 0x02                 ; FAT表的记录数
BPB_RootEntCnt dw 0x00e0               ; 最大根目录文件数
BPB_TotSec16   dw 0x0b40               ; 逻辑扇区总数
BPB_Media      db 0xf0                 ; 媒体描述符
BPB_FATSz16    dw 0x0009               ; 每个FAT占用扇区数
BPB_SecPerTrk  dw 0x0012               ; 每个磁道扇区数
BPB_NumHeads   dw 0x0002               ; 磁头数
BPB_HiddSec    dd 0x00000000           ; 隐藏扇区数
BPB_TotSec32   dd 0x00000000           ; 如果BPB_TotSec16是0，则在这里记录扇区总数
BS_DrvNum      db 0x00                 ; 中断13的驱动器号
BS_Reserved1   db 0x00                 ; 未使用
BS_BootSig     db 0x29                 ; 扩展引导标志
BS_VollD       dd 0x00000000           ; 卷序列号
BS_VolLab      db "H.H.OS-0.01"        ; 卷标，必须是11个字符，不足以空格填充
BS_FileSysTyep db "FAT12   "           ; 文件系统类型，必须是8个字符，不足填充空格

; code entry
BOOT_CODE:                             ; 引导代码，由偏移0字节处的短跳转而来
	jmp BLMain

; loading the target file
; return : 
; 		dx --> 0 : failure !0 : success
loadTarget:
	push ax
	push bx
	push cx

	push si
	push di

	push es
	push bp

	mov ax, RootEntryOffset
	mov cx, RootEntryLength
	mov bx, Buffer
	call readSector

	mov si, TargetStr
	mov cx, TargetLen
	xor dx, dx
	call findEntry

	cmp dx, 0
	jz loadTargetEnd

	mov si, bx
	mov di, BaseOfStack
	mov cx, DIR_Length
	call memCpy

	mov ax, FatTableOffset
	mov cx, FatTableLength
	mov bx, FAT1Address
	call readSector

	mov bp, BaseOfStack
	mov dx, word [bp + DIR_FstClus]
	mov si, LoadAddress
	shr si, 4
	mov es, si
	xor si, si
	call loadFile

loadTargetEnd:
	pop bp
	pop es

	pop di
	pop si

	pop cx
	pop bx
	pop ax

	ret

; loading file to memory
; dx --> sector number
loadFile:
	push ax
	push bx
	push cx
	push dx
	push si

load:
	cmp dx, 0x0ff7
	jnb loadFileEnd

	mov ax, dx
	add ax, 0x1f

	push dx

	mov bx, si
	mov cx, 0x01
	call readSector

	pop bx
	call fatVec
	
	add si, word [BPB_BytsPerSec]
	cmp si, 0
	jne load

	mov si, es
	add si, 0x1000
	mov es, si

	xor si, si
	jmp load
	
loadFileEnd:
	pop si
	pop dx
	pop cx
	pop bx
	pop ax

	ret

; in fat table find sector number
; bx --> index
; return:
; 	dx --> sector number
fatVec:
	push ax
	push bx
	push si
	push di
	push bp 

	mov bp, sp

	xor dx, dx
	mov ax, bx
	shr ax, 0x01

	mov bx, 0x03
	mul bx

	mov si, ax
	mov di, ax
	xor dx, dx
	
	mov bx, FAT1Address

	mov ax, word [bp + 0x06]
	test ax, 0x01
	jz evenNumber

; 奇数
oddNumber:
	add si, 0x02

	inc di

	mov dl, byte [bx + si]
	shl dx, 0x04

	xor ax, ax
	mov al, byte [bx + di]
	shr al, 0x04
	and al, 0x0f

	or dx, ax

	jmp fatVecEnd

; 偶数
evenNumber:	
	inc si

	mov dl, byte [bx + si]
	and dl, 0x0f
	shl dx, 0x08

	mov dl, byte [bx + di]

fatVecEnd:
	pop bp
	pop di
	pop si
	pop bx
	pop ax

	ret


; in root entry find target
; es:bx --> root entry offset address
; ds:si --> target string
; cx    --> target length
;
; return:
;   (dx != 0) ? exist : noexist
;       exist --> bx is the target entry
findEntry:	
	push cx
	push si
	push bp

	mov bp, sp

	mov dx, word [BPB_RootEntCnt]

find:
	mov si, bx
	call memCmp

	cmp cx, 0x00
	jz exist

	dec dx
	cmp dx, 0x00
	jz noexist

	add bx, DIR_Length
	mov cx, word [bp + 0x04]
	mov di, word [bp + 0x02]
	jmp find

noexist:
exist:
	pop bp
	pop si
	pop cx

	ret

; memory copy
; ds:si --> source
; es:di --> destination
; cx    --> length
memCpy:
	push cx
	push si
	push di

	cmp si, di
	ja bToe

; end to begin
eTob:
	add si, cx
	add di, cx
	dec si
	dec di

	std
	rep movsb

	jmp memCpyEnd
	
; begin to end
bToe:
	cld
	rep movsb

memCpyEnd:
	pop di
	pop si
	pop cx

	ret

; memory compare
; ds:si --> source
; es:di --> destination
; cx    --> length
;
; reture:
;	(cx == 0) ? equal : noequal
memCmp:
	push si
	push di

	cld
	repe cmpsb
	jz equal

noequal:
	mov cx, 0xffff

equal:
	pop di
	pop si
	ret

; reset Floppy disk
; no parameter
resetFloppy:
	push ax
	push dx

	xor ah, ah
	mov dl, byte [BS_DrvNum]
	int 0x13

	pop dx
	pop ax

	ret

; read Sector to memory
; ax    --> logic sector number
; cx    --> number of sector
; es:bx --> target address
readSector:

	push ax
	push bx
	push cx
	push dx
	push bp

	mov bp, sp

	call resetFloppy

	xor dx, dx
	mov bx, word [BPB_SecPerTrk]
	div bx

	mov cx, dx
	inc cl

	mov dh, al
	and dh, 0x01

	mov ch, al
	shr ch, 0x01

	mov ax, word [bp + 0x04]
	mov ah, 0x02

	mov dl, byte [BS_DrvNum]
	mov bx, word [bp + 0x06]

read:
	int 0x13
	jc read

	pop bp
	pop dx
	pop cx
	pop bx
	pop ax

	ret	

; print error info
Error:
	mov bp, ErrorStr
	mov cx, ErrorLen

	call printString

last:
	hlt
	jmp last

; print string
; es:bp --> string address
; cx    --> string length
printString:
	push ax
	push bx
	push dx

	xor dx, dx
	mov ax, 0x1301
	mov bx, 0x0007

	int 0x10

	pop dx
	pop bx
	pop ax

	ret	

; end of blfunc.asm	
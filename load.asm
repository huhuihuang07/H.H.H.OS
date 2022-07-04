BootAddress equ 0x9000

org BootAddress

%include "blfunc.asm"
%include "common.asm"

interface:
	MagicNumber equ 0x7c00
	BaseOfStack equ MagicNumber - DIR_Length
	LoadAddress equ 0xa000

	TargetStr db  "KERNEL  BIN"
	TargetLen equ $ - TargetStr

[section .gdt]
; GDT definition
;                                           段基址         段界限               段属性
GDT_ENTRY           :      Descriptor         0,            0,                  0
FLAT_MODE_CODE_DESC :      Descriptor         0,         0xfffff,          DA_32 + DA_C + DA_LIMIT_4K + DA_DPL0
VIDEO32_DESC        :      Descriptor     0xB8000,       0x07FFF,          DA_32 + DA_DRWA + DA_DPL3
; end of global descriptor table

GdtLen         equ          $ - GDT_ENTRY
GdtPtr:
				dw           GdtLen - 1 ; GDT 界限
				dd           0          ; GDT 基地址

; GDT Selector
FlatModeCodeSelector      equ (0x0001 << 3) + SA_TIG + SA_RPL0
Video32Selector           equ (0x0002 << 3) + SA_TIG + SA_RPL3
; end of [section .gdt]

[section .idt]
; IDT definition
;                          选择子         偏移地址            参数个数         段属性
IDT_ENTRY:        
%rep 256
	Gate                     0,             0,                0,           DA_386IGate + DA_DPL3             
%endrep                                 

; end of interrupt descriptor table

IdtLen      equ         $ - IDT_ENTRY
IdtPtr:
			dw          IdtLen - 1 ; IDT 界限
			dd          0          ; IDT 基地址

; end of [section .idt]			

[section .s16]
[bits 16]
BLMain:

	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax

	mov sp, BaseOfStack

	; loading kernel to memory
	call loadTarget

	cmp dx, 0
	jz Error

	; initialize GDT for 32 bits code segment

	; initialize GDT pointer struct
	xor eax, eax
	mov ax, ds
	shl eax, 0x04
	add eax, GDT_ENTRY
	mov dword [GdtPtr + 0x02], eax

	; initialize IDT pointer struct
	xor eax, eax
	mov ax, ds
	shl eax, 0x04
	add eax, IDT_ENTRY
	mov dword [IdtPtr + 0x02], eax

	; 1. load GDT and load IDT
	lgdt [GdtPtr]
	lidt [IdtPtr]

	; 2. close interrupt
	cli

	; 3. open A20
	in al, 0x92
	or al, 0000_0010b
	out 0x92, al

	; 4. enter protect mode
	mov eax, cr0
	or eax, 0x01
	mov cr0, eax

	; 5. jump to 32 bits code
	jmp dword FLAT_MODE_CODE_DESC : LoadAddress

; initialize descriptor item
; esi --> code segment label
; edi --> descriptor label
InitDescItem:
	push eax
	
	xor eax, eax
	mov ax, cs
	shl eax, 0x04
	add eax, esi
	mov word [edi + 0x02], ax
	shr eax, 0x10
	mov byte [edi + 0x04], al
	mov byte [edi + 0x07], ah	

	pop eax

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

	xor dx,dx
	mov ax, 0x1301
	mov bx, 0x0007

	int 0x10

	pop dx
	pop bx
	pop ax

	ret	

; end of [section .s16]

[section .errorStr]
[bits 16]
Error_Segment:

	ErrorStr db "Not find kernel ..."
	ErrorLen equ $ - ErrorStr

; end of [section .errorStr]	

[section .buffer]
[bits 16]
Buffer_Segment:
	Buffer db 0x00

; end of [section .buffer]
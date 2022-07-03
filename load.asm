BootAddress equ 0x9000

org BootAddress

%include "blfunc.asm"
%include "common.asm"

interface:
	BaseOfStack equ BootAddress - DIR_Length
	LoadAddress equ 0xA000

	TargetStr db  "KERNEL  BIN"
	TargetLen equ $ - TargetStr

[section .gdt]
; GDT definition
;                                      段基址         段界限               段属性
GDT_ENTRY      :      Descriptor         0,            0,                  0
CODE32_DESC    :      Descriptor         0,      Code32SegLen - 1,    DA_32 + DA_C
STACK32_DESC   :      Descriptor         0,      Stack32SegLen - 1,   DA_32 + DA_DRW
DATA32_DESC    :      Descriptor         0,      Data32SegLen - 1,    DA_32 + DA_DR
VIDEO32_DESC   :      Descriptor     0xB8000,       0x07FFF,          DA_32 + DA_DRWA
CODE16_DESC    :      Descriptor         0,          0xffff,          DA_C
UPDATE_DESC    :      Descriptor         0,          0xffff,          DA_DRWA
TASKALDT_DESC  :      Descriptor         0,      TASKALDTSegLen - 1,  DA_32 + DA_LDT
FUNCTION_DESC  :      Descriptor         0,      FunctionSegLen - 1,  DA_32 + DA_C
TSS_DESC       :      Descriptor         0,      	TSSLen - 1,       DA_32 + DA_386TSS
; Gate definition
;                                             选择子                 偏移地址            参数个数          属性
PRINTSTRING_GATE       :      Gate       FunctionSelector,     PrintStringOffset,       0,        DA_386CGate
; GDT end

GdtLen         equ          $ - GDT_ENTRY
GdtPtr:
				dw           GdtLen - 1 ; GDT 界限
				dd           0          ; GDT 基地址

; GDT Selector
Code32Selector      equ (0x0001 << 3) + SA_TIG + SA_RPL0
Stack32Selector     equ (0x0002 << 3) + SA_TIG + SA_RPL0
Data32Selector      equ (0x0003 << 3) + SA_TIG + SA_RPL0
Video32Selector     equ (0x0004 << 3) + SA_TIG + SA_RPL0
Code16Selector      equ (0x0005 << 3) + SA_TIG + SA_RPL0
UpdateSelector      equ (0x0006 << 3) + SA_TIG + SA_RPL0
TaskALDTSelector    equ (0x0007 << 3) + SA_TIG + SA_RPL0
FunctionSelector    equ (0x0008 << 3) + SA_TIG + SA_RPL0
TSSSelector         equ (0x0009 << 3) + SA_TIG + SA_RPL0
PrintStringSelector equ (0x000a << 3) + SA_TIG + SA_RPL0
; end of [section .gdt]

[section .tss]
[bits 32]
TSS_SEGMENT:
	dd 0
	dd TopOfStackInit32 ; 0
	dd Stack32Selector  ;
	dd 0                ; 1
	dd 0                ;
	dd 0                ; 2
	dd 0                ; 
	times 4 * 18 dd 0
	dw 0
	dw $ - TSS_SEGMENT
	db 0xff

TSSLen equ $ - TSS_SEGMENT
; end of [section .tss]

TopOfStackInit16 equ 0x7c00

[section .s16]
[bits 16]
BLMain:

	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax

	mov word [BACK_TO_REAL_MODE + 0x03], ax

	mov sp, TopOfStackInit16

	; initialize GDT for 32 bits code segment
	mov esi, CODE32_SEGMENT
	mov edi, CODE32_DESC
	call InitDescItem

	mov esi, STACK32_SEGMENT
	mov edi, STACK32_DESC
	call InitDescItem

	mov esi, DATA32_SEGMENT
	mov edi, DATA32_DESC
	call InitDescItem

	mov esi, CODE16_SEGMENT
	mov edi, CODE16_DESC
	call InitDescItem

	mov esi, FUNCTION_SEGMENT
	mov edi, FUNCTION_DESC
	call InitDescItem

	mov esi, TASKALDT_SEGMENT
	mov edi, TASKALDT_DESC
	call InitDescItem

	mov esi, TSS_SEGMENT
	mov edi, TSS_DESC
	call InitDescItem

	mov esi, TASK_A_CODE_SEGMENT
	mov edi, TASK_A_CODE_DESC
	call InitDescItem

	mov esi, TASK_A_STACK_SEGMENT
	mov edi, TASK_A_STACK_DESC
	call InitDescItem

	mov esi, TASK_A_DATA_SEGMENT
	mov edi, TASK_A_DATA_DESC
	call InitDescItem

	; initialize GDT pointer struct
	xor eax, eax
	mov ax, ds
	shl eax, 0x04
	add eax, GDT_ENTRY
	mov dword [GdtPtr + 0x02], eax

	; 1. load GDT
	lgdt [GdtPtr]

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
	jmp dword Code32Selector : 0

BACK_ENTRY_SEGMENT:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ss, ax

	mov sp, TopOfStackInit16

	; open interrupt
	sti

	; close A20
	in al, 0x92
	and al, 1111_1101b
	out 0x92, al

	mov bp, HHOS
	mov cx, 0x07
	mov ax, 0x1301
	mov bx, 0x0007

	xor dx, dx

	int 0x10

	jmp $

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

; end of [section .s16]

[section .c16]
[bits 16]
CODE16_SEGMENT:
	mov ax, UpdateSelector
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ss, ax

	mov eax, cr0
	and eax, 1111_1110b
	mov cr0, eax

BACK_TO_REAL_MODE:
	jmp 0 : BACK_ENTRY_SEGMENT

Code16SegLen               equ              $ - CODE16_SEGMENT
; end of [section .c16]

[section .s32]
[bits 32]
CODE32_SEGMENT:
	mov ax, Stack32Selector
	mov ss, ax

	mov sp, TopOfStackInit32

	mov ax, Data32Selector
	mov ds, ax

	mov ebp, HHOS_OFFSET

	mov dh, 0x0d
	mov dl, 0x20
	mov bh, 0x0c
	call PrintStringSelector : 0

	mov ax, TaskALDTSelector
	lldt ax

	mov ax, TSSSelector
	ltr ax

	jmp TaskACodeSelector : 0

Code32SegLen         equ        $ - CODE32_SEGMENT
; end of [section .s32]

[section .gs32]
[bits 32]
STACK32_SEGMENT:
	times 4 * 1024 db 0

Stack32SegLen    equ $ - STACK32_SEGMENT
TopOfStackInit32 equ Stack32SegLen - 1
; end of [section .s32]

[section .data32]
[bits 32]
DATA32_SEGMENT:
	HHOS db "H.H.OS!", 0x00
	HHOS_OFFSET equ HHOS - $$
	
Data32SegLen equ $ - DATA32_SEGMENT
; end of [section .data32]

;========================================
;
;              Function
;
;========================================
[section .function]
[bits 32]
FUNCTION_SEGMENT:

; print string (25 行 * 80 列)
; ds:ebp --> string address
; bh     --> string attribute
; dx     --> dh : row, dl : col
PrintString:
	push eax
	push ecx
	push edx
	push edi
	push ebp

	mov ax, Video32Selector
	mov gs, ax

	xor edi, edi
	xor eax, eax

	mov al, dh
	mov cx, 0x50
	mul cx

	mov edx, [esp + 0x08]
	xor dh, dh
	add ax, dx

	shl eax, 0x01
	mov edi, eax

	mov ah, bh

print:	
	mov al, byte [ds:ebp]
	cmp al, 0x00
	jz PrintStringEnd

	mov word [gs:edi], ax

	add edi, 0x02
	inc ebp
	jmp print

PrintStringEnd:

	pop ebp
	pop edi
	pop edx
	pop ecx
	pop eax

	retf

PrintStringOffset equ PrintString - $$

FunctionSegLen equ $ - FUNCTION_SEGMENT
; end of [section .function]

;========================================
;
;              Task A Code
;
;========================================

[section .task-a-ldt]
[bits 32]
TASKALDT_SEGMENT:
; LDT definition
;                                                          段基址               段界限                      段属性
TASK_A_CODE_DESC          :           Descriptor             0,        TaskACodeSegLen - 1,           DA_32 + DA_C
TASK_A_STACK_DESC         :           Descriptor             0,        TaskAStackSegLen - 1,          DA_32 + DA_DRW
TASK_A_DATA_DESC          :           Descriptor             0,        TaskADataSegLen - 1,           DA_32 + DA_DRW

TASKALDTSegLen equ $ - TASKALDT_SEGMENT

; LDT Selector
TaskACodeSelector  equ (0x0000 << 3) + SA_TIL + SA_RPL0
TaskAStackSelector equ (0x0001 << 3) + SA_TIL + SA_RPL0
TaskADataSelector  equ (0x0002 << 3) + SA_TIL + SA_RPL0

; end of [section .task-a-ldt]

[section .task-a-code]
[bits 32]
TASK_A_CODE_SEGMENT:
	mov ax, TaskADataSelector
	mov ds, ax

	mov ax, TaskAStackSelector
	mov ss, ax

	mov sp, TopOfStackAInit32

	mov ebp, TaskAOffset

	mov dh, 0x02
	mov dl, 0x00
	mov bh, 0x0c
	call PrintStringSelector : 0

	jmp Code16Selector : 0

TaskACodeSegLen equ $ - TASK_A_CODE_SEGMENT
; end of [section .task-a-code]

[section .task-a-stack]
[bits 32]
TASK_A_STACK_SEGMENT:
	times 4 * 1024 db 0x00

TaskAStackSegLen  equ $ - TASK_A_STACK_SEGMENT
TopOfStackAInit32 equ TaskAStackSegLen - 1
; end of [section .task-a-stack]

[section .task-a-data]
[bits 32]
TASK_A_DATA_SEGMENT:
	TaskAString       db         "This is Task A ...",0x00
	TaskAOffset       equ        TaskAString - $$           

TaskADataSegLen equ $ - TASK_A_DATA_SEGMENT
; end of [section .task-a-data]

[section .buffer]
[bits 16]
Buffer db 0x00
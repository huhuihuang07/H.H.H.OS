%include "blfunc.asm"
%include "common.asm"

org BaseOfLoader

interface:
	BaseOfStack equ BaseOfBoot - DIR_Length
	LoadAddress equ BaseOfKernel

	TargetStr db  "KERNEL  BIN"
	TargetLen equ $ - TargetStr

[section .gdt]
; GDT definition
;                                           段基址         段界限               段属性
GDT_ENTRY           :      Descriptor         0,            0,                  0
FLAT_MODE_CODE_DESC :      Descriptor         0,         0xfffff,          DA_32 + DA_C + DA_LIMIT_4K + DA_DPL0
VIDEO32_DESC        :      Descriptor     0xB8000,       0x07FFF,          DA_32 + DA_DRWA + DA_DPL0
KERNELDATA_DESC     :      Descriptor         0,     KernelDataLen - 1,    DA_32 + DA_DRWA + DA_DPL0
FLAT_MODE_DATA_DESC :      Descriptor         0,         0xfffff,          DA_32 + DA_DRWA + DA_LIMIT_4K + DA_DPL0
CODE32_DESC         :      Descriptor         0,   Code32SegmentLen - 1,   DA_32 + DA_C + DA_DPL0
LDT_DESC            :      Descriptor         0,            0,             0
TSS_DESC            :      Descriptor         0,            0,             0
; end of global descriptor table

GdtLen         equ          $ - GDT_ENTRY

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
; end of [section .idt]		

[section .kernelData]
[bits 32]
KERNELDATA_SEGMENT:
	; GDT pointer
	GdtPtr:
			dw          GdtLen - 1 ; GDT 界限
			dd          0          ; GDT 基地址

	; IDT pointer		
	IdtPtr:
			dw          IdtLen - 1 ; IDT 界限
			dd          0          ; IDT 基地址


	; Memory size
	MemorySize       dd  0

	; Memory usage information
	ARDSNumber       db  0
	ARDSPointer:	
		times 5 * 20 dd  0

KernelDataLen     equ       $ - KERNELDATA_SEGMENT	
; end of [section .kernelData]	

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
	jnz Enter

	mov bp, FindErrorStr
	mov cx, FindErrorLen
	jmp print

Enter:
	mov eax, LoadAddress
	add eax, dword [BaseOfStack + DIR_FileSize]
	cmp eax, BaseOfAPP
	jna Next

	mov bp, LenErrorStr
	mov cx, LenErrorLen
	jmp print

Next:
	call getMemorySize

	; initialize GDT for 32 bits code segment
	mov esi, KERNELDATA_SEGMENT
	mov edi, KERNELDATA_DESC
	call InitDescItem

	mov esi, CODE32_SEGMENT
	mov edi, CODE32_DESC
	call InitDescItem

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

	; set IOPL to 3
	pushf
	pop eax

	or eax, 0x3000

	push eax
	popf

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

; get Memory Size
getMemorySize:
	push eax
	push ebx
	push ecx

	push edi

; get memory size
eax_0xe801:
	mov eax, 0xe801

	int 0x15

	jc getError

	shl eax, 10 ; 15M以下内存容量（单位1Kb）

	shl ebx, 16 ; 16M以上内存容量（单位64Kb）

	mov ecx, 1  ; 1M的内存黑洞
	shl ecx, 20

	add eax, ebx
	add eax, ecx

	mov di, MemorySize

	mov dword [di], eax

; get detailed memory usage information
eax_0xe820:
	xor ebx, ebx	
	mov di, ARDSPointer

getMemoryInfo:
	mov eax, 0xe820
	mov edx, 0x534d4150 ; "SMAP"
	mov ecx, 0x14

	int 0x15

	jc getError

	add di, 0x14

	inc byte [ARDSNumber]

	cmp ebx, 0
	jnz getMemoryInfo

getError:
	pop edi 

	pop ecx
	pop ebx
	pop eax

	ret	

; end of [section .s16]

[section .errorStr]
[bits 16]
Error_Segment:

	FindErrorStr db "Not find kernel ..."
	FindErrorLen equ $ - FindErrorStr

	LenErrorStr db "Kernel size out of range ..."
	LenErrorLen equ $ - LenErrorStr

; end of [section .errorStr]	

[section .code32]
[bits 32]
CODE32_SEGMENT:
	mov ax, Video32Selector
	mov gs, ax

	mov ax, FlatModeDataSelector
	mov ds, ax
	mov es, ax
	mov fs, ax

	mov ss, ax
	mov esp, BaseOfLoader

	jmp dword FlatModeCodeSelector : BaseOfKernel
Code32SegmentLen     equ       $ - CODE32_SEGMENT
; end of [section .code32]

[section .buffer]
[bits 16]
Buffer_Segment:
	Buffer db 0x00

; end of [section .buffer]
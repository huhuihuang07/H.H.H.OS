; global variable
BaseOfBoot   equ 0x7c00
BaseOfLoader equ 0x9000
BaseOfKernel equ 0xa000
BaseOfAPP    equ 0x20000

; Segment Attribute 段属性
DA_32       equ 0x4000 ; 保护模式下32位段
DA_DR       equ 0x90   ; 只读数据段
DA_DRW      equ 0x92   ; 可读写数据段
DA_DRWA     equ 0x93   ; 已访问可读写数据段
DA_C        equ 0x98   ; 只执行代码段
DA_CR       equ 0x9a   ; 可执行可读代码段
DA_CCO      equ 0x9c   ; 只执行一致代码段
DA_CCOR     equ 0x9e   ; 可执行可读一致代码段
DA_LIMIT_4K equ 0x8000 ; 单位大小4K

; Segment Pirvilege 段特权级
DA_DPL0 equ 0x00 ; DPL = 0
DA_DPL1 equ 0x20 ; DPL = 1
DA_DPL2 equ 0x40 ; DPL = 2
DA_DPL3 equ 0x60 ; DPL = 3

; Special Attribute
DA_LDT equ 0x82 ; 局部段描述符表 

; Gate Attribute 门描述符属性
DA_TaskGate equ 0x85 ; 任意门类型值
DA_386TSS   equ 0x89 ; 可以用 386 任务状态段类型值
DA_386CGate equ 0x8c ; 386 调用门类型值
DA_386IGate equ 0x8e ; 386 中断门类型值
DA_386TGate equ 0x8f ; 386 陷进门类型值

; Selector Attribute 选择子属性
; Selector Privilege 选择子特权级
SA_RPL0 equ 0 ; RPL = 0
SA_RPL1 equ 1 ; RPL = 1
SA_RPL2 equ 2 ; RPL = 2
SA_RPL3 equ 3 ; RPL = 3

SA_TIG equ 0 ; GDT 全局段描述符表
SA_TIL equ 4 ; LDT 局部段描述符表

; GDT Selector
FlatModeCodeSelector   equ (0x0001 << 3) + SA_TIG + SA_RPL0
Video32Selector        equ (0x0002 << 3) + SA_TIG + SA_RPL3
KernelDataSelector     equ (0x0003 << 3) + SA_TIG + SA_RPL0
FlatModeDataSelector   equ (0x0004 << 3) + SA_TIG + SA_RPL0
Code32Selector         equ (0x0005 << 3) + SA_TIG + SA_RPL0
LdtSelector            equ (0x0006 << 3) + SA_TIG + SA_RPL0
TssSelector            equ (0x0007 << 3) + SA_TIG + SA_RPL0

; 描述符
; usage: Descriptor Base, Limit, Attribute
;        Base:      dd
;        Limit:     dd (low 20 bits available)
;        Attribute: dw (lower 4 bits of higher byte are always 0)
%macro Descriptor 3                         ; 段基址， 段界限， 段属性
	dw %2 & 0xffff					       	; 段界限1	
	dw %1 & 0xffff                          ; 段基址1
	db (%1 >> 16) & 0xff                    ; 段基址2
	dw ((%2 >> 8) & 0x0f00) | (%3 & 0xf0ff) ; 属性1 + 段界限2 + 属性2
	db (%1 >> 24) & 0xff                    ; 段基址3
%endmacro                                   ; 共8字节

; 门描述符
; usage: Gate Selector, offset, DCount, Attribute
;		 Selector:  dw
;		 offset:    dd
;		 DCount:    db
;		 Attribute: db
%macro Gate 4                                ; 选择子， 偏移地址， 参数个数， 属性
	dw %2 & 0xffff                           ; 偏移地址1
	dw %1 								     ; 选择子
	dw ((%4 << 8) & 0xff00) | (%3 & 0x001f)  ; 属性 + 参数个数
	dw (%2 >> 16) & 0xffff                   ; 偏移地址2
%endmacro                                    ; 共8字节
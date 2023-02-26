#ifndef CONST_H
#define CONST_H

// global variable
#define BaseOfBoot 0x7c00
#define BaseOfLoader 0x9000
#define BaseOfKernel 0xa000

// Segment Attribute 段属性
#define DA_32 0x4000       // 保护模式下32位段
#define DA_DR 0x90         // 只读数据段
#define DA_DRW 0x92        // 可读写数据段
#define DA_DRWA 0x93       // 已访问可读写数据段
#define DA_C 0x98          // 只执行代码段
#define DA_CR 0x9a         // 可执行可读代码段
#define DA_CCO 0x9c        // 只执行一致代码段
#define DA_CCOR 0x9e       // 可执行可读一致代码段
#define DA_LIMIT_4K 0x8000 // 单位大小4K

// Segment Pirvilege 段特权级
#define DA_DPL0 0x00 // DPL = 0
#define DA_DPL1 0x20 // DPL = 1
#define DA_DPL2 0x40 // DPL = 2
#define DA_DPL3 0x60 // DPL = 3

#define SA_RPL_MASK 0xfffc
#define SA_TI_MASK 0xfffb

// Special Attribute
#define DA_LDT 0x82 // 局部段描述符表

// Gate Attribute 门描述符属性
#define DA_TaskGate 0x85 // 任意门类型值
#define DA_386TSS 0x89   // 可以用 386 任务状态段类型值
#define DA_386CGate 0x8c // 386 调用门类型值
#define DA_386IGate 0x8e // 386 中断门类型值
#define DA_386TGate 0x8f // 386 陷进门类型值

// Selector Attribute 选择子属性
// Selector Privilege 选择子特权级
#define SA_RPL0 0 // RPL = 0
#define SA_RPL1 1 // RPL = 1
#define SA_RPL2 2 // RPL = 2
#define SA_RPL3 3 // RPL = 3

#define SA_TIG 0 // GDT 全局段描述符表
#define SA_TIL 4 // LDT 局部段描述符表

#define GDT_UndefinedIndex 0x0000
#define GDT_FlatModeCodeIndex 0x0001
#define GDT_Video32Index 0x0002
#define GDT_KernelDataIndex 0x0003
#define GDT_FlatModeDataIndex 0x0004
#define GDT_Code32Index 0x0005
#define GDT_LDTIndex 0x0006
#define GDT_TSSIndex 0x0007

// GDT Selector GDT 选择子
#define GDT_UndefinedSelector (GDT_UndefinedIndex << 3) + SA_TIG + SA_RPL0
#define GDT_FlatModeCodeSelector (GDT_FlatModeCodeIndex << 3) + SA_TIG + SA_RPL0
#define GDT_Video32Selector (GDT_Video32Index << 3) + SA_TIG + SA_RPL0
#define GDT_KernelDataSelector (GDT_KernelDataIndex << 3) + SA_TIG + SA_RPL0
#define GDT_FlatModeDataSelector (GDT_FlatModeDataIndex << 3) + SA_TIG + SA_RPL0
#define GDT_Code32Selector (GDT_Code32Index << 3) + SA_TIG + SA_RPL0
#define GDT_LdtSelector (GDT_LDTIndex << 3) + SA_TIG + SA_RPL0
#define GDT_TssSelector (GDT_TSSIndex << 3) + SA_TIG + SA_RPL0

#define LDT_Code32Index 0x0000
#define LDT_Data32Index 0x0001
#define LDT_Stack32Index 0x0002

// LDT Selector LDT 选择子
#define LDT_Code32Selector (LDT_Code32Index << 3) + SA_TIL + SA_RPL3
#define LDT_Data32Selector (LDT_Data32Index << 3) + SA_TIL + SA_RPL3
#define LDT_Stack32Selector (LDT_Stack32Index << 3) + SA_TIL + SA_RPL3

#endif //! CONST_H
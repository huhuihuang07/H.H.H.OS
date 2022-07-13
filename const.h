#ifndef CONST_H
#define CONST_H

// PIC-8259A Ports
#define MASTER_ICW1_PORT  0x20
#define MASTER_ICW2_PORT  0x21
#define MASTER_ICW3_PORT  0x21
#define MASTER_ICW4_PORT  0x21
#define MASTER_OCW1_PORT  0x21
#define MASTER_OCW2_PORT  0x20
#define MASTER_OCW3_PORT  0x20

#define SLAVE_ICW1_PORT   0xa0
#define SLAVE_ICW2_PORT   0xa1
#define SLAVE_ICW3_PORT   0xa1
#define SLAVE_ICW4_PORT   0xa1
#define SLAVE_OCW1_PORT   0xa1
#define SLAVE_OCW2_PORT   0xa0
#define SLAVE_OCW3_PORT   0xa0

#define MASTER_EOI_PORT   0x20
#define MASTER_IMR_PORT   0x21
#define MASTER_IRR_PORT   0x20
#define MASTER_ISR_PORT   0x20

#define SLAVE_EOI_PORT    0xa0
#define SLAVE_IMR_PORT    0xa1
#define SLAVE_IRR_PORT    0xa0
#define SLAVE_ISR_PORT    0xa0

// Segment Attribute 段属性
#define DA_32        0x4000 // 保护模式下32位段
#define DA_DR        0x90   // 只读数据段
#define DA_DRW       0x92   // 可读写数据段
#define DA_DRWA      0x93   // 已访问可读写数据段
#define DA_C         0x98   // 只执行代码段
#define DA_CR        0x9a   // 可执行可读代码段
#define DA_CCO       0x9c   // 只执行一致代码段
#define DA_CCOR      0x9e   // 可执行可读一致代码段
#define DA_LIMIT_4K  0x8000 // 单位大小4K

// Segment Pirvilege 段特权级
#define DA_DPL0  0x00 // DPL = 0
#define DA_DPL1  0x20 // DPL = 1
#define DA_DPL2  0x40 // DPL = 2
#define DA_DPL3  0x80 // DPL = 3

#define SA_RPL_MASK 0xfffc
#define SA_TI_MASK  0xfffb

// Special Attribute
#define DA_LDT       0x82 // 局部段描述符表 

// Gate Attribute 门描述符属性
#define DA_TaskGate  0x85 // 任意门类型值
#define DA_386TSS    0x89 // 可以用 386 任务状态段类型值
#define DA_386CGate  0x8c // 386 调用门类型值
#define DA_386IGate  0x8e // 386 中断门类型值
#define DA_386TGate  0x8f // 386 陷进门类型值

// Selector Attribute 选择子属性
// Selector Privilege 选择子特权级
#define SA_RPL0  0 // RPL = 0
#define SA_RPL1  1 // RPL = 1
#define SA_RPL2  2 // RPL = 2
#define SA_RPL3  3 // RPL = 3

#define SA_TIG  0 // GDT 全局段描述符表
#define SA_TIL  4 // LDT 局部段描述符表

// GDT Selector GDT 选择子
#define FlatModeCodeSelector    (0x0001 << 3) + SA_TIG + SA_RPL0
#define Video32Selector         (0x0002 << 3) + SA_TIG + SA_RPL0
#define KernelDataSelector      (0x0003 << 3) + SA_TIG + SA_RPL0
#define FlatModeDataSelector    (0x0004 << 3) + SA_TIG + SA_RPL0
#define Code32Selector          (0x0005 << 3) + SA_TIG + SA_RPL0

#endif //!CONST_H
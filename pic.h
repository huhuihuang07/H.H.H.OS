#ifndef PIC_H
#define PIC_H

#include "type.h"

// PIC-8259A Ports
#define MASTER_ICW1_PORT 0x20
#define MASTER_ICW2_PORT 0x21
#define MASTER_ICW3_PORT 0x21
#define MASTER_ICW4_PORT 0x21
#define MASTER_OCW1_PORT 0x21
#define MASTER_OCW2_PORT 0x20
#define MASTER_OCW3_PORT 0x20

#define SLAVE_ICW1_PORT 0xa0
#define SLAVE_ICW2_PORT 0xa1
#define SLAVE_ICW3_PORT 0xa1
#define SLAVE_ICW4_PORT 0xa1
#define SLAVE_OCW1_PORT 0xa1
#define SLAVE_OCW2_PORT 0xa0
#define SLAVE_OCW3_PORT 0xa0

#define MASTER_EOI_PORT 0x20
#define MASTER_IMR_PORT 0x21
#define MASTER_IRR_PORT 0x20
#define MASTER_ISR_PORT 0x20

#define SLAVE_EOI_PORT 0xa0
#define SLAVE_IMR_PORT 0xa1
#define SLAVE_IRR_PORT 0xa0
#define SLAVE_ISR_PORT 0xa0

#define IRQ_CLOCK 0      // 时钟
#define IRQ_KEYBOARD 1   // 键盘
#define IRQ_CASCADE 2    // 8259 从片控制器
#define IRQ_SERIAL_2 3   // 串口 2
#define IRQ_SERIAL_1 4   // 串口 1
#define IRQ_PARALLEL_2 5 // 并口 2
#define IRQ_FLOPPY 6     // 软盘控制器
#define IRQ_PARALLEL_1 7 // 并口 1
#define IRQ_RTC 8        // 实时时钟
#define IRQ_REDIRECT 9   // 重定向 IRQ2
#define IRQ_MOUSE 12     // 鼠标
#define IRQ_MATH 13      // 协处理器 x87
#define IRQ_HARDDISK 14  // ATA 硬盘第一通道
#define IRQ_HARDDISK2 15 // ATA 硬盘第二通道

#define IRQ_MASTER_NR 0x20 // 主片起始向量号
#define IRQ_SLAVE_NR 0x28  // 从片起始向量号

// End Of Interrupt
#define PIC_EOI 0x20 // 通知中断控制器中断结束
// Close All Interrupt
#define PIC_CAI 0xff // 关闭所有中断

void InitPIC();
u8 ReadIMR(u16 port);
void WriteIMR(u16 port, u8 value);
void SendEOI(u16 port);

void SetInterruptMask(u32 irq, State state);
State SetIFState(State state);
State DisableIF();
State EnableIF();
State GetIFState();

#endif //! PIC_H

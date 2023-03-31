#include "pic.h"
#include "utility.h"
#include "assert.h"
#include "io.h"

uint8_t ReadIMR(uint16_t port)
{
    return inb(port);
}

void WriteIMR(uint16_t port, uint8_t value)
{
    outb(port, value);
}

void SendEOI(uint16_t port)
{
    outb(port, PIC_EOI);
}

state_t GetIFState()
{
    uint32_t eflags = 0u;

    asm volatile(
        "pushfl        \n"
        "popl     %%eax\n"
        "movl %%eax, %0\n"
        : "=r"(eflags)
        :
        : "eax");

    return TestBit(eflags, 9u);
}

state_t SetIFState(state_t state)
{
    state_t ret = GetIFState();

    if (IsEqual(state, Enable) && !IsEqual(ret, state))
    {
        asm volatile("sti");
    }

    if (IsEqual(state, Disable) && !IsEqual(ret, state))
    {
        asm volatile("cli");
    }

    return ret;
}

state_t DisableIF()
{
    return SetIFState(Disable);
}

state_t EnableIF()
{
    return SetIFState(Enable);
}

void SetInterruptMask(uint32_t irq, state_t state)
{
    assert((IRQ_CLOCK <= irq) && (irq <= IRQ_HARDDISK2));

    uint16_t port = irq < IRQ_RTC ? MASTER_IMR_PORT : (irq -= IRQ_RTC, SLAVE_IMR_PORT);

    if (IsEqual(state, Enable))
    {
        WriteIMR(port, ClearBit(ReadIMR(port), irq));

        if (IsEqual(port, SLAVE_IMR_PORT))
        {
            uint16_t value = ReadIMR(MASTER_IMR_PORT);

            if (TestBit(value, IRQ_CASCADE))
            {
                WriteIMR(MASTER_IMR_PORT, ClearBit(value, IRQ_CASCADE));
            }
        }
    }
    else
    {
        WriteIMR(port, SetBit(ReadIMR(port), irq));

        if (IsEqual(port, SLAVE_IMR_PORT))
        {
            uint16_t value = ReadIMR(SLAVE_IMR_PORT);

            if (IsEqual(value, PIC_CAI))
            {
                WriteIMR(MASTER_IMR_PORT, SetBit(ReadIMR(MASTER_IMR_PORT), IRQ_CASCADE));
            }
        }
    }
}

void InitPIC()
{
    // initialize Master 8259A
    outb(MASTER_ICW1_PORT, 0b00010001);    // ICW1: 边沿触发, 级联 8259, 需要ICW4.
    outb(MASTER_ICW2_PORT, IRQ_MASTER_NR); // ICW2: 起始中断向量号 0x20
    outb(MASTER_ICW3_PORT, 0b00000100);    // ICW3: IR2接从片.
    outb(MASTER_ICW4_PORT, 0b00010001);    // ICW4: 特殊全嵌套, 非缓冲数据连接, 手动结束中断

    // initialize Slave 8259A
    outb(SLAVE_ICW1_PORT, 0b00010001);   // ICW1: 边沿触发, 级联 8259, 需要ICW4.
    outb(SLAVE_ICW2_PORT, IRQ_SLAVE_NR); // ICW2: 起始中断向量号 0x28
    outb(SLAVE_ICW3_PORT, 0b00000010);   // ICW3: 设置从片连接到主片的 IR2 引脚
    outb(SLAVE_ICW4_PORT, 0b00000001);   // ICW4: 普通全嵌套, 非缓冲数据连接, 手动结束中断

    // close all interrupt
    WriteIMR(MASTER_IMR_PORT, PIC_CAI);

    WriteIMR(SLAVE_IMR_PORT, PIC_CAI);

    // open IF
    SetIFState(Enable);
}

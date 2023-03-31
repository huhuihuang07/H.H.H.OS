#include "interrupt.h"
#include "utility.h"
#include "assert.h"
#include "kernel.h"
#include "pic.h"

bool SetInterruptGate(uint8_t InterruptVectorNumber, uint32_t Handlerfunc)
{
    return SetGateValue(AddrOffset(gIdtInfo.entry, InterruptVectorNumber), GDT_FlatModeCodeSelector, Handlerfunc, 0u, DA_386IGate + DA_DPL3);
}

bool SetInterruptHandler(uint8_t irq, uint32_t Handlerfunc)
{
    assert((IRQ_CLOCK <= irq) && (irq <= IRQ_HARDDISK2));

    return SetInterruptGate(IRQ_MASTER_NR + irq, Handlerfunc);
}

bool InterruptGateInit()
{
    bool ret = true;

    for (uint16_t i = 0; (!IsEqual(i, ENTRY_SIZE)) && ret; ++i)
    {
        ret = SetInterruptGate(i, handler_entry_table[i]);
    }

    return ret;
}

void InterruptModuleInit()
{
    InterruptGateInit();

    InitPIC();
}
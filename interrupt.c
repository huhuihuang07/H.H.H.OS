#include <interrupt.h>
#include <utility.h>
#include <assert.h>
#include <kernel.h>
#include <8259A.h>

bool SetInterruptGate(u8 InterruptVectorNumber, u32 Handlerfunc)
{
	return SetGateValue(AddrOffset(gIdtInfo.entry, InterruptVectorNumber), GDT_FlatModeCodeSelector, Handlerfunc, 0, DA_386IGate + DA_DPL3);
}

bool SetInterruptHandler(u8 irq, u32 Handlerfunc)
{
	assert((IRQ_CLOCK <= irq) && (irq <= IRQ_HARDDISK2));

	return SetInterruptGate(IRQ_MASTER_NR + irq, Handlerfunc);
}

bool InterruptGateInit()
{
	bool ret = true;

	for(u16 i = 0; (!IsEqual(i, gIdtInfo.size)) && ret; ++i)
	{
		ret = SetInterruptGate(i, (u32)(DefaultHandlerEntry));
	}

	return ret;
}

void InterruptModuleInit()
{
	InterruptGateInit();

	DebugInit();

	Init8259A();
}
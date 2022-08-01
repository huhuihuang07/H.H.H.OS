#ifndef IHANDLER_H
#define IHANDLER_H

#include <interrupt.h>
#include <8259A.h>

#define TimerInterrupt      0x20
#define TimerMasterIMR      0xfe
#define TimerSlaveIMR       0xff

#define DebugInterrupt      0x03
#define DebugMasterIMR      0xff
#define DebugSlaveIMR       0xff

#define SysCallInterrupt    0x80
#define SysCallMasterIMR    0xff
#define SysCallSlaveIMR     0xff

#ifndef DeclHandler
#define DeclHandler(name) 	             \
	extern void name##HandlerEntry();    \
	void name##Init()                    \
	{                                    \
		SetInterruptGate(name##Interrupt, (u32)name##HandlerEntry); \
                                                                    \
		u8 al = ReadIMR(MASTER_IMR_PORT);                           \
                                                                    \
		al &= name##MasterIMR;                                      \
                                                                    \
		WriteIMR(MASTER_IMR_PORT, al);                              \
                                                                    \
		al = ReadIMR(SLAVE_IMR_PORT);                               \
                                                                    \
		al &= name##SlaveIMR;                                       \
                                                                    \
		WriteIMR(SLAVE_IMR_PORT, al);                               \
	}                                                               \
	void name##Handler()
#endif

DeclHandler(Timer);
DeclHandler(Debug);
DeclHandler(SysCall);

extern void SystemCall(u32 param);

#endif //!IHANDLER_H
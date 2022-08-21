#ifndef IHANDLER_H
#define IHANDLER_H

#include <interrupt.h>
#include <8259A.h>

// define External interrupt
#define TimerInterrupt      0x20
#define TimerMasterIMR      0xfe
#define TimerSlaveIMR       0xff

// define Internal interrupt
#define DebugInterrupt      0x03
#define SysCallInterrupt    0x80

#ifndef DeclExternalInterrupt
#define DeclExternalInterrupt(name) 	 \
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

#ifndef DeclInternalInterrupt
#define DeclInternalInterrupt(name)      \
	extern void name##HandlerEntry();    \
	void name##Init()                    \
	{                                    \
		SetInterruptGate(name##Interrupt, (u32)name##HandlerEntry); \
	}                                                               \
	u32 name##Handler()
#endif	

DeclExternalInterrupt(Timer);

DeclInternalInterrupt(Debug);
DeclInternalInterrupt(SysCall);

extern void SystemCall(u32 param);
extern u32 TaskCallHandler(u32 cmd, u32 param1, u32 param2);

#endif //!IHANDLER_H
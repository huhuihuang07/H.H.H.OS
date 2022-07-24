#ifndef IHANDLER_H
#define IHANDLER_H

#include "interrupt.h"
#include "8259A.h"

#define TimerInterrupt      0x20
#define TimerMasterIMR      0xfe
#define TimerSlaveIMR       0xff

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

#endif //!IHANDLER_H
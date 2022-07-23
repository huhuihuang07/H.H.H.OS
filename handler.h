#ifndef HANDLER_H
#define HANDLER_H

#include "interrupt.h"

#define TimerInterrupt      0x20
#define TimerMasterIMR      0xfe
#define TimerSlaveIMR       0xff

#ifndef Interrupt
#define Interrupt(name) 	             \
	void name##Init();                   \
	void name##Handler();                \
	extern void name##HandlerEntry()
#endif

Interrupt(Timer);

#endif //!HANDLER_H
#ifndef IHANDLER_H
#define IHANDLER_H

#include "interrupt.h"
#include "8259A.h"

// define External interrupt
#define TimerInterrupt      IRQ_CLOCK

// define Internal interrupt
#define DebugInterrupt      0x03
#define SysCallInterrupt    0x80

#define PageFaultInterrupt  0x0e

#ifndef DeclExternalInterrupt
#define DeclExternalInterrupt(name) 	 \
	extern void name##HandlerEntry();    \
	void name##Init()                    \
	{                                    \
		SetInterruptHandler(name##Interrupt, (u32)name##HandlerEntry); \
                                                                       \
        SetInterruptMask(name##Interrupt, Enable);                     \
	}                                                                  \
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

#ifndef DeclInternalFault
#define DeclInternalFault(name)      \
	extern void name##HandlerEntry();    \
	void name##Init()                    \
	{                                    \
		SetInterruptGate(name##Interrupt, (u32)name##HandlerEntry); \
	}                                                               \
	void name##Handler(u32 error)
#endif	

// Declare External interrupt handler
DeclExternalInterrupt(Timer);

// Declare Internal interrupt handler
DeclInternalInterrupt(Debug);
DeclInternalInterrupt(SysCall);

// Declare Internal fault handler
DeclInternalFault(PageFault);

void DefaultInterruptHandler();
void DefaultFaultHandler(u32 error);

extern void SystemCall(u32 param);
extern u32 TaskCallHandler(u32 cmd, u32 param1, u32 param2);
extern void PageFault(u32 error);

#endif //!IHANDLER_H
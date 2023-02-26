#ifndef IHANDLER_H
#define IHANDLER_H

#include "interrupt.h"
#include "pic.h"

// define External interrupt
#define TimerInterrupt IRQ_CLOCK

// define Internal interrupt
#define SysCallInterrupt 0x80

#define PageFaultInterrupt 0x0e

typedef enum
{
	Systerm = 0,
	User = 3,
} PrivilegeLevel;

#define INTERRUPT_NUM 256

typedef void (*handlerFun)(u32 vector, u32 error_code, PrivilegeLevel privilegeLevel);

void DefaultInterruptHandler(u32 vector, u32 error_code, PrivilegeLevel privilegeLevel);

handlerFun handler_table[INTERRUPT_NUM] = {DefaultInterruptHandler};

#ifndef DeclExternalInterrupt
#define DeclExternalInterrupt(name)                                                \
	void name##Handler(u32 vector, u32 error_code, PrivilegeLevel privilegeLevel); \
	void name##Init()                                                              \
	{                                                                              \
		handler_table[IRQ_MASTER_NR + name##Interrupt] = name##Handler;            \
                                                                                   \
		SetInterruptMask(name##Interrupt, Enable);                                 \
	}
#endif

#ifndef DeclInternalInterrupt
#define DeclInternalInterrupt(name)                                                \
	void name##Handler(u32 vector, u32 error_code, PrivilegeLevel privilegeLevel); \
	void name##Init()                                                              \
	{                                                                              \
		handler_table[name##Interrupt] = name##Handler;                            \
	}
#endif

// Declare External interrupt handler
DeclExternalInterrupt(Timer)

	// Declare Internal fault handler
	DeclInternalInterrupt(PageFault)

		extern void SystemCall(u32 param);
extern u32 TaskCallHandler(u32 cmd, u32 param1, u32 param2);
extern u32 ScreenCallHandler(u32 cmd, u32 param1, u32 param2);
extern void PageFault(u32 error);

#endif //! IHANDLER_H
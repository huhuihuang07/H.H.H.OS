#pragma once

#include "interrupt.h"
#include "pic.h"

// define External interrupt
#define TimerInterrupt    IRQ_CLOCK
#define KeyBoardInterrupt IRQ_KEYBOARD

// define Internal interrupt
#define SysCallInterrupt 0x80

#define PageFaultInterrupt 0x0e

typedef enum {
    Kernel = 0,
    User   = 3,
} PrivilegeLevel_t;

#define INTERRUPT_NUM 256u

typedef void (*handlerFun)(uint32_t vector, uint32_t error_code, PrivilegeLevel_t privilegeLevel);

void DefaultInterruptHandler(uint32_t vector, uint32_t error_code, PrivilegeLevel_t privilegeLevel);

handlerFun handler_table[INTERRUPT_NUM] = {DefaultInterruptHandler};

#ifndef DeclExternalInterrupt
#define DeclExternalInterrupt(name)                                                            \
    void name##Handler(uint32_t vector, uint32_t error_code, PrivilegeLevel_t privilegeLevel); \
    void name##Init()                                                                          \
    {                                                                                          \
        handler_table[IRQ_MASTER_NR + name##Interrupt] = name##Handler;                        \
                                                                                               \
        SetInterruptMask(name##Interrupt, Enable);                                             \
    }
#endif

#ifndef DeclInternalInterrupt
#define DeclInternalInterrupt(name)                                                            \
    void name##Handler(uint32_t vector, uint32_t error_code, PrivilegeLevel_t privilegeLevel); \
    void name##Init()                                                                          \
    {                                                                                          \
        handler_table[name##Interrupt] = name##Handler;                                        \
    }
#endif

extern void SystemCall(uint32_t param);
extern uint32_t TaskCallHandler(uint32_t cmd, uint32_t param1, uint32_t param2);
extern uint32_t ScreenCallHandler(uint32_t cmd, uint32_t param1, uint32_t param2);
extern void PageFault(uint32_t error);

// clang-format off

// Declare External interrupt handler
DeclExternalInterrupt(Timer)
DeclExternalInterrupt(KeyBoard)

// Declare Internal fault handler
DeclInternalInterrupt(PageFault)
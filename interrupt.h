#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "type.h"

#ifndef FaultSize
#define FaultSize 0x16
#endif

#define ENTRY_SIZE 0x30

extern u32 handler_entry_table[ENTRY_SIZE];

void InterruptModuleInit();

bool InterruptGateInit();

bool SetInterruptGate(u8 InterruptVectorNumber, u32 Handlerfunc);

bool SetInterruptHandler(u8 irq, u32 Handlerfunc);

#endif //!INTERRUPT_H
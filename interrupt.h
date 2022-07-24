#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "type.h"

extern void DefaultHandlerEntry();

void InterruptModuleInit();

bool InterruptGateInit();

bool SetInterruptGate(u8 InterruptVectorNumber, u32 Handlerfunc);

#endif //!INTERRUPT_H
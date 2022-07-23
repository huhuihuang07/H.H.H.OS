#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "type.h"
#include "global.h"

extern void DefaultHandlerEntry();

void InitInterrupt();

bool InitInterruptGate();

bool SetInterruptGate(u8 InterruptVectorNumber, u32 Handlerfunc);

#endif //!INTERRUPT_H
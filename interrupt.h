#pragma once

#include "type.h"

#ifndef FaultSize
#define FaultSize 0x16
#endif

#define ENTRY_SIZE 0x30

extern uint32_t handler_entry_table[ENTRY_SIZE];

void InterruptModuleInit();

bool InterruptGateInit();

bool SetInterruptGate(uint8_t InterruptVectorNumber, uint32_t Handlerfunc);

bool SetInterruptHandler(uint8_t irq, uint32_t Handlerfunc);
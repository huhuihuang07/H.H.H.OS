#pragma once

#include "type.h"

uint32_t CreateMutex();
void EnterCritical(uint32_t mutex);
void ExitCritical(uint32_t mutex);
void DestroyMutex(uint32_t mutex);
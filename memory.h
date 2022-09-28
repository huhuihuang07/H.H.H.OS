#ifndef MEMORY_H 
#define MEMORY_H

#include "pMemory.h"
#include "vMemory.h"

void MemoryModuleInit();

void* malloc(size_t size);
void free(const void* ptr);

#endif //!MEMORY_H
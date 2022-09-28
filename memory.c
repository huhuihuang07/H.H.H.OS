#include "memory.h"

void MemoryModuleInit()
{
	pMemoryModuleInit();

	vMemoryModuleInit();
}

void* malloc(size_t size)
{
	return pMalloc(size);
}

void free(const void* ptr)
{
	pFree(ptr);
}
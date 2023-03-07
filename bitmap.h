#pragma once

#include "type.h"

typedef struct
{
	uint8_t *bits;
	uint32_t length;
} BitMap_t;

void BitMap_Init(BitMap_t *bitmap, void *bits, uint32_t length);

bool BitMap_Test(BitMap_t *bitmap, uint32_t index);

void BitMap_Set(BitMap_t *bitmap, uint32_t index, bool value);

bool BitMap_Scan(BitMap_t *bitmap, uint32_t count, uint32_t *start);
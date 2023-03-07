#pragma once

#include "type.h"

typedef struct
{
	u8 *bits;
	u32 length;
} BitMap;

void BitMap_Init(BitMap *bitmap, void *bits, u32 length);

bool BitMap_Test(BitMap *bitmap, u32 index);

void BitMap_Set(BitMap *bitmap, u32 index, bool value);

bool BitMap_Scan(BitMap *bitmap, u32 count, u32 *start);
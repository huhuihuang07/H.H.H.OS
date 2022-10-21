#include "bitmap.h"
#include "string.h"
#include "assert.h"
#include "utility.h"

void BitMap_Init(BitMap* bitmap, void* bits, u32 length)
{
	assert((!IsEqual(bitmap, nullptr)) && (!IsEqual(bits, nullptr)) && (length > 0));

	bitmap->bits = bits;

	bitmap->length = length;

	memset(bitmap->bits, 0, bitmap->length);
}

bool BitMap_Test(BitMap* bitmap, u32 index)
{
	assert(!IsEqual(bitmap, nullptr));

	u32 bytes = index >> 3;

	u8 bits = index & 0x07;

	assert(bytes < bitmap->length);

	return TestBit(bitmap->bits[bytes], bits);
}

void BitMap_Set(BitMap* bitmap, u32 index, bool value)
{
	assert(!IsEqual(bitmap, nullptr));

	u32 bytes = index >> 3;

	u8 bits = index & 0x07;

	assert(bytes < bitmap->length);

	bitmap->bits[bytes] = IsEqual(value, true) ? SetBit(bitmap->bits[bytes], bits) : ClearBit(bitmap->bits[bytes], bits);
}

bool BitMap_Scan(BitMap* bitmap, u32 count, u32* start)
{
	assert((!IsEqual(bitmap, nullptr)) && (IsEqual(start, nullptr)) && (count > 0) && (count <= (bitmap->length << 3)));

	u32 bytes = 0;

	while(IsEqual(bitmap->bits[bytes], 0xff) && (bytes < bitmap->length))
	{
		++bytes;
	}

	if(IsEqual(bytes, bitmap->length))
	{
		return false;
	}

	u8 bits = 0;

	while(TestBit(bitmap->bits[bytes], bits))
	{
		++bits;
	}

	u32 index = bytes << 3 + bits;

	if(IsEqual(count, 1))
	{
		*start = index;
		BitMap_Set(bitmap, index, true);
		return true;
	}

	u32 left = bitmap->length << 3 - index;

	u32 nextIndex = index + 1;

	u32 counter = 1;

	while(!IsEqual(left--, 0))
	{
		if(BitMap_Test(bitmap, nextIndex))
		{
			counter = 0;
		}else{
			counter++;
		}

		if(IsEqual(counter, count))
		{
			*start = nextIndex - count + 1;
			break;
		}

		nextIndex++;
	}

	if(!IsEqual(counter, count))
	{
		return false;
	}

	while(!IsEqual(counter, 0))
	{
		BitMap_Set(bitmap, *start + counter - 1, true);

		counter--;
	}

	return true;
}
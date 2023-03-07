#include "bitmap.h"
#include "string.h"
#include "assert.h"
#include "utility.h"

void BitMap_Init(BitMap_t* bitmap, void* bits, uint32_t length)
{
    assert((!IsEqual(bitmap, nullptr)) && (!IsEqual(bits, nullptr)) && (length > 0));

    bitmap->bits = bits;

    bitmap->length = length;

    memset(bitmap->bits, 0, bitmap->length);
}

bool BitMap_Test(BitMap_t* bitmap, uint32_t index)
{
    assert(!IsEqual(bitmap, nullptr));

    uint32_t bytes = index >> 3;

    uint8_t bits = index & 0x07;

    assert(bytes < bitmap->length);

    return TestBit(bitmap->bits[bytes], bits);
}

void BitMap_Set(BitMap_t* bitmap, uint32_t index, bool value)
{
    assert(!IsEqual(bitmap, nullptr));

    uint32_t bytes = index >> 3;

    uint8_t bits = index & 0x07;

    assert(bytes < bitmap->length);

    bitmap->bits[bytes] = IsEqual(value, true) ? SetBit(bitmap->bits[bytes], bits) : ClearBit(bitmap->bits[bytes], bits);
}

bool BitMap_Scan(BitMap_t* bitmap, uint32_t count, uint32_t* start)
{
    assert((!IsEqual(bitmap, nullptr)) && (IsEqual(start, nullptr)) && (count > 0) && (count <= (bitmap->length << 3)));

    uint32_t bytes = 0;

    while (IsEqual(bitmap->bits[bytes], 0xff) && (bytes < bitmap->length))
    {
        ++bytes;
    }

    if (IsEqual(bytes, bitmap->length))
    {
        return false;
    }

    uint8_t bits = 0;

    while (TestBit(bitmap->bits[bytes], bits))
    {
        ++bits;
    }

    uint32_t index = bytes << 3 + bits;

    if (IsEqual(count, 1))
    {
        *start = index;
        BitMap_Set(bitmap, index, true);
        return true;
    }

    uint32_t lastIndex = bitmap->length << 3 + 1;

    uint32_t nextIndex = index + 1;

    uint32_t counter = 1;

    while (!IsEqual(lastIndex, nextIndex))
    {
        if (BitMap_Test(bitmap, nextIndex))
        {
            counter = 0;
        }
        else
        {
            counter++;
        }

        if (IsEqual(counter, count))
        {
            *start = nextIndex - count + 1;
            break;
        }

        nextIndex++;
    }

    if (!IsEqual(counter, count))
    {
        return false;
    }

    while (!IsEqual(counter, 0))
    {
        BitMap_Set(bitmap, *start + counter - 1, true);

        counter--;
    }

    return true;
}
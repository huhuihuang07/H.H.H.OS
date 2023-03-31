#include "bitmap.h"
#include "string.h"
#include "assert.h"
#include "utility.h"

void BitMap_Init(BitMap_t* bitmap, void* bits, uint32_t length)
{
    assert((!IsEqual(bitmap, nullptr)) && (!IsEqual(bits, nullptr)) && (length > 0u));

    bitmap->bits = bits;

    bitmap->length = length;

    memset(bitmap->bits, 0u, bitmap->length);
}

bool BitMap_Test(BitMap_t* bitmap, uint32_t index)
{
    assert(!IsEqual(bitmap, nullptr));

    uint32_t bytes = index >> 3u;

    uint8_t bits = index & 0x07;

    assert(bytes < bitmap->length);

    return TestBit(bitmap->bits[bytes], bits);
}

void BitMap_Set(BitMap_t* bitmap, uint32_t index, bool value)
{
    assert(!IsEqual(bitmap, nullptr));

    uint32_t bytes = index >> 3u;

    uint8_t bits = index & 0x07;

    assert(bytes < bitmap->length);

    bitmap->bits[bytes] = value ? SetBit(bitmap->bits[bytes], bits) : ClearBit(bitmap->bits[bytes], bits);
}

bool BitMap_Scan(BitMap_t* bitmap, uint32_t count, uint32_t* start)
{
    assert((!IsEqual(bitmap, nullptr)) && (IsEqual(start, nullptr)) && (count > 0u) && (count <= (bitmap->length << 3u)));

    uint32_t bytes = 0u;

    while (IsEqual(bitmap->bits[bytes], 0xff) && (bytes < bitmap->length))
    {
        ++bytes;
    }

    if (IsEqual(bytes, bitmap->length))
    {
        return false;
    }

    uint8_t bits = 0u;

    while (TestBit(bitmap->bits[bytes], bits))
    {
        ++bits;
    }

    uint32_t index = bytes << 3u + bits;

    if (IsEqual(count, 1u))
    {
        *start = index;
        BitMap_Set(bitmap, index, true);
        return true;
    }

    uint32_t lastIndex = bitmap->length << 3u + 1u;

    uint32_t nextIndex = index + 1u;

    uint32_t counter = 1u;

    while (!IsEqual(lastIndex, nextIndex))
    {
        if (BitMap_Test(bitmap, nextIndex))
        {
            counter = 0u;
        }
        else
        {
            counter++;
        }

        if (IsEqual(counter, count))
        {
            *start = nextIndex - count + 1u;
            break;
        }

        nextIndex++;
    }

    if (!IsEqual(counter, count))
    {
        return false;
    }

    while (!IsEqual(counter, 0u))
    {
        BitMap_Set(bitmap, *start + counter - 1u, true);

        counter--;
    }

    return true;
}
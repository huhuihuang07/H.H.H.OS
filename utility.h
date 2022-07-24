#ifndef UTILITY_H
#define UTILITY_H

#include "type.h"

#ifndef AddrOffset
#define AddrOffset(p, i) ((void*)(u32)(p) + (i) * sizeof(*p))
#endif

void Delay(u8 n);
#endif //!UTILITY_H
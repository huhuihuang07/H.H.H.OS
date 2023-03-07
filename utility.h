#pragma once

#include "type.h"

#ifndef AddrOffset
#define AddrOffset(p, i) \
	((void *)((u32)(p) + (i) * sizeof(*(p))))
#endif

#ifndef AddrIndex
#define AddrIndex(b, a) \
	(((u32)(a) - (u32)(b)) / sizeof(*(b)))
#endif

#ifndef ArraySize
#define ArraySize(a) \
	(sizeof(a) / sizeof(*(a)))
#endif

#ifndef offsetof
#define offsetof(TYPE, MEMBER) \
	((size_t) & ((TYPE *)nullptr)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, TYPE, MEMBER) \
	((TYPE *)((char *)(ptr)-offsetof(TYPE, MEMBER)))
#endif

#ifndef StructOffset
#define StructOffset(ptr, TYPE, MEMBER) \
	((void *)((u32)(ptr) + offsetof(TYPE, MEMBER)))
#endif

#ifndef IsEqual
#define IsEqual(a, b) ({         \
	unsigned ta = (unsigned)(a); \
	unsigned tb = (unsigned)(b); \
	!(ta - tb);                  \
})
#endif

#ifndef RoundDown
#define RoundDown(a, n) ({      \
	size_t ta = (size_t)(a);    \
	(typeof(a))(ta - ta % (n)); \
})
#endif

#ifndef RoundUp
#define RoundUp(a, n) ({                              \
	size_t tn = (size_t)(n);                          \
	(typeof(a))(RoundDown((size_t)(a) + tn - 1, tn)); \
})
#endif

#ifndef SetBit
#define SetBit(value, pos) \
	((value) | (1 << (pos)))
#endif

#ifndef ClearBit
#define ClearBit(value, pos) \
	((value) & (~(1 << (pos))))
#endif

#ifndef TestBit
#define TestBit(value, pos) \
	(IsEqual(((value) >> (pos)) & 0x01, 0x01))
#endif

#ifndef Max
#define Max(a, b) ({         \
	signed ta = (signed)(a); \
	signed tb = (signed)(b); \
	(ta >= tb) ? ta : tb;    \
})
#endif

#ifndef Min
#define Min(a, b) ({         \
	signed ta = (signed)(a); \
	signed tb = (signed)(b); \
	(ta <= tb) ? ta : tb;    \
})
#endif

#ifndef Clamp
#define Clamp(v, a, b) ({    \
	signed ta = (signed)(a); \
	signed tb = (signed)(b); \
	signed tv = (signed)(v); \
	Min(tb, Max(tv, ta));    \
})
#endif

#ifndef UTILITY_H
#define UTILITY_H

#include <type.h>

#ifndef AddrOffset
#define AddrOffset(p, i) \
		((void*)((u32)(p) + (i) * sizeof(*p)))
#endif

#ifndef AddrIndex
#define AddrIndex(b, a) \
		(((u32)(a) - (u32)(b)) / sizeof(*b))
#endif		

#ifndef offsetof
#define offsetof(TYPE, MEMBER) \
		((size_t)&((TYPE*)nullptr)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, TYPE, MEMBER) \
		((TYPE *)((char*)(ptr) - offsetof(TYPE, MEMBER)))
#endif	

#ifndef StructOffset
#define StructOffset(ptr, TYPE, MEMBER) \
		((void*)((u32)(ptr) + offsetof(TYPE, MEMBER))) 
#endif	

#ifndef IsEqual
#define IsEqual(a, b) ({              \
		unsigned ta = (unsigned)(a);  \
		unsigned tb = (unsigned)(b);  \
		!(ta - tb);                   \
	})
#endif

#ifndef Max
#define Max(a, b) \
	((a) >= (b) ? (a) : (b))
#endif

#ifndef Min
#define Min(a, b) \
	((a) <= (b) ? (a) : (b))
#endif

void Delay(u8 n);
#endif //!UTILITY_H
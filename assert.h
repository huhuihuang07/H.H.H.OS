#ifndef ASSERT_H
#define ASSERT_H

#include "type.h"

void assertionFailure(const char *exp, const char *file, u32 line);

#ifndef assert
#ifdef NODEBUG
#define assert(exp)
#else
#define assert(exp) \
	if (exp)        \
		;           \
	else            \
		assertionFailure(#exp, __FILE__, __LINE__)
#endif
#endif

#endif //! ASSERT_H
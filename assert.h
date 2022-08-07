#ifndef ASSERT_H
#define ASSERT_H

void assertionFailure(const char *exp, const char *file, int line);

#ifndef assert
#define assert(exp) \
		if(exp)     \
		;           \
		else        \
			assertionFailure(#exp, __FILE__,  __LINE__)
#endif

#endif //!ASSERT_H
#pragma once

#include "type.h"

void AssertionFailure(const char* exp, const char* file, uint32_t line);

#ifndef assert
#ifdef NODEBUG
#define assert(exp)
#else
#define assert(exp) \
    if (exp)        \
        ;           \
    else            \
        AssertionFailure(#exp, __FILE__, __LINE__)
#endif
#endif
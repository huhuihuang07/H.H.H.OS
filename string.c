#include "string.h"
#include "memory.h"
#include "assert.h"
#include "utility.h"

char* strcpy(char* dest, const char* src)
{
    assert((!IsEqual(dest, nullptr)) && (!IsEqual(src, nullptr)));

    char* lptr = dest;
    char* rptr = (char*)src;

    while (!IsEqual(*lptr++ = *rptr++, EOS))
        ;

    return dest;
}

char* strncpy(char* dest, const char* src, size_t n)
{
    assert((!IsEqual(dest, nullptr)) && (!IsEqual(src, nullptr)));

    char* lptr = dest;
    char* rptr = (char*)src;

    while ((!IsEqual(*lptr++ = *rptr++, EOS)) && (!IsEqual(n--, 0)))
        ;

    if (!IsEqual(n, 0))
    {
        while ((!IsEqual(n--, 0)))
        {
            *lptr++ = EOS;
        }
    }

    return dest;
}

char* strcat(char* dest, const char* src)
{
    assert((!IsEqual(dest, nullptr)) && (!IsEqual(src, nullptr)));

    char* ptr = AddrOffset(dest, strlen(dest));

    strcpy(ptr, src);

    return dest;
}

char* strncat(char* dest, const char* src, size_t n)
{
    assert((!IsEqual(dest, nullptr)) && (!IsEqual(src, nullptr)));

    char* ptr = AddrOffset(dest, strlen(dest));

    strncpy(ptr, src, n);

    return dest;
}

size_t strlen(const char* str)
{
    assert((!IsEqual(str, nullptr)));

    char* ptr = (char*)str;

    while (!IsEqual(*ptr++, EOS))
        ;

    return AddrIndex(str, ptr) - 1;
}

int32_t strcmp(const char* lhs, const char* rhs)
{
    assert((!IsEqual(lhs, nullptr)) && (!IsEqual(rhs, nullptr)));

    while (IsEqual(*lhs, *rhs) && (!IsEqual(*lhs, EOS)) && (!IsEqual(*rhs, EOS)))
    {
        lhs++;
        rhs++;
    }

    return *lhs < *rhs ? -1 : *lhs > *rhs;
}

int32_t strncmp(const char* lhs, const char* rhs, size_t n)
{
    assert((!IsEqual(lhs, nullptr)) && (!IsEqual(rhs, nullptr)));

    while ((!IsEqual(n--, 0)) && IsEqual(*lhs, *rhs) && (!IsEqual(*lhs, EOS)) && (!IsEqual(*rhs, EOS)))
    {
        lhs++;
        rhs++;
    }

    return *lhs < *rhs ? -1 : *lhs > *rhs;
}

char* strchr(const char* str, int32_t ch)
{
    assert((!IsEqual(str, nullptr)));

    char* ptr = (char*)str;

    while ((!IsEqual(*ptr, ch)) && (!IsEqual(*ptr, EOS)))
    {
        ++ptr;
    }

    return IsEqual(*ptr, ch) ? ptr : nullptr;
}

char* strrchr(const char* str, int32_t ch)
{
    assert((!IsEqual(str, nullptr)));

    char* ptr = AddrOffset(str, strlen(str));

    while ((!IsEqual(*ptr, ch)) && (!IsEqual(str, ptr)))
    {
        --ptr;
    }

    return IsEqual(*ptr, ch) ? ptr : nullptr;
}

int32_t memcmp(const void* lhs, const void* rhs, size_t count)
{
    assert((!IsEqual(lhs, nullptr)) && (!IsEqual(rhs, nullptr)));

    char* lptr = (char*)lhs;
    char* rptr = (char*)rhs;

    while ((!IsEqual(count--, 0)) && IsEqual(*lptr++, *rptr++))
        ;

    return *lptr < *rptr ? -1 : *lptr > *rptr;
}

void* memset(void* dest, int32_t ch, size_t count)
{
    assert((!IsEqual(dest, nullptr)));

    char* ptr = (char*)dest;

    while (!IsEqual(count--, 0))
    {
        *ptr++ = ch;
    }

    return dest;
}

void* memcpy(void* dest, const void* src, size_t count)
{
    assert((!IsEqual(dest, nullptr)) && (!IsEqual(src, nullptr)));

    char* lptr = (char*)dest;
    char* rptr = (char*)src;

    if (lptr <= rptr)
    {
        while ((!IsEqual(count--, 0)))
        {
            *lptr++ = *rptr++;
        }
    }
    else
    {
        lptr += count - 1;
        rptr += count - 1;

        while ((!IsEqual(count--, 0)))
        {
            *lptr-- = *rptr--;
        }
    }

    return dest;
}

void* memchr(const void* str, int32_t ch, size_t count)
{
    assert((!IsEqual(str, nullptr)));

    char* ptr = (char*)str;

    while ((!IsEqual(count--, 0)) && (!IsEqual(*ptr, ch)))
    {
        ++ptr;
    }

    return IsEqual(*ptr, ch) ? ptr : nullptr;
}

char* strdup(const char* src)
{
    assert(!IsEqual(src, nullptr));

    char* ret = malloc(strlen(src) + 1);

    return strcpy(ret, src);
}

char* strndup(const char* src, size_t n)
{
    assert(!IsEqual(src, nullptr));

    char* ret = malloc(n + 1);

    return strncpy(ret, src, n + 1);
}

static int32_t* make_pmt(const char* str)
{
    int32_t len = strlen(str);

    int32_t* ret = (int32_t*)malloc(sizeof(int32_t) * len);

    int32_t ll = 0;

    ret[0] = ll;

    for (int32_t i = 1; !IsEqual(i, len); ++i)
    {
        while ((!IsEqual(ll, 0)) && (!IsEqual(str[i], str[ll])))
        {
            ll = ret[ll - 1];
        }

        if (IsEqual(str[i], str[ll]))
        {
            ++ll;
        }

        ret[i] = ll;
    }

    return ret;
}

int32_t strfind(const char* src, const char* needle)
{
    assert((!IsEqual(src, nullptr)) && (!IsEqual(needle, nullptr)));

    int32_t sLen = strlen(src), fLen = strlen(needle), i = 0, j = 0;

    if ((fLen <= 0) || (fLen > sLen))
    {
        return -1;
    }

    int32_t* pmt = make_pmt(needle);

    for (i = 0, j = 0; (!IsEqual(i, fLen)) && (!IsEqual(j, sLen)); ++j)
    {
        while ((!IsEqual(i, 0) && (!IsEqual(src[j], needle[i]))))
        {
            i = pmt[i - 1];
        }

        if (IsEqual(src[j], needle[i]))
        {
            ++i;
        }
    }

    free(pmt);

    return IsEqual(i, fLen) ? j - fLen : -1;
}

char* strstr(const char* src, const char* needle)
{
    assert((!IsEqual(src, nullptr)) && (!IsEqual(needle, nullptr)));

    int32_t index = strfind(src, needle);

    return IsEqual(index, -1) ? nullptr : AddrOffset(src, index);
}
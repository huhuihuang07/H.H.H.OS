#include "string.h"
#include "memory.h"
#include "assert.h"
#include "utility.h"

char* strcpy(char* dest, const char* src)
{
	assert((!IsEqual(dest, nullptr)) && (!IsEqual(src, nullptr)));

	char* lptr = dest;
	char* rptr = (char*)src;

	while(*lptr++ = *rptr++);

	return dest;
}

char* strncpy(char* dest, const char* src, size_t strlen)
{
	assert((!IsEqual(dest, nullptr)) && (!IsEqual(src, nullptr)));

	char* lptr = dest;
	char* rptr = (char*)src;

	while((*lptr++ = *rptr++) && !IsEqual(strlen, 0))
	{
		strlen--;
	}

	if(!IsEqual(strlen, 0))
	{
		while(--strlen){
			*lptr++ = EOS;
		}
	}

	return dest;
}

char* strcat(char* dest, const char* src)
{
	assert((!IsEqual(dest, nullptr)) && (!IsEqual(src, nullptr)));

	char* ptr = dest + strlen(dest);

	strcpy(ptr, src);

	return dest;
}

size_t strlen(const char* str)
{
	assert((!IsEqual(str, nullptr)));

	char* ptr = (char*)str;

	while(*ptr++);

	return (ptr - str) - 1;
}

int strcmp(const char* lhs, const char* rhs)
{
	assert((!IsEqual(lhs, nullptr)) && (!IsEqual(rhs, nullptr)));

	while((IsEqual(*lhs, *rhs)) && (*lhs) && (*rhs)){
		lhs++;
		rhs++;
	}

	return *lhs < *rhs ? -1 : *lhs > *rhs;
}

char* strchr(const char* str, int ch)
{
	assert((!IsEqual(str, nullptr)));

	char* ptr = (char*)str;

	while((!IsEqual(*ptr, ch)) && (*ptr)){
		++ptr;
	}

	return IsEqual(*ptr, ch) ? ptr : nullptr;
}

char* strrchr(const char* str, int ch)
{
	assert((!IsEqual(str, nullptr)));

	char* ptr = (char*)str + strlen(str);

	while((!IsEqual(*ptr, ch)) && (!IsEqual(str, ptr))){
		--ptr;
	}

	return IsEqual(*ptr, ch) ? ptr : nullptr;
}

int memcmp(const void* lhs, const void* rhs, size_t count)
{
	assert((!IsEqual(lhs, nullptr)) && (!IsEqual(rhs, nullptr)));

	char* lptr = (char*)lhs;
	char* rptr = (char*)rhs;

	while((count --) && (*lptr++ = *rptr++));

	return *lptr < *rptr ? -1 : *lptr > *rptr;
}

void* memset(void* dest, int ch, size_t count)
{
	assert((!IsEqual(dest, nullptr)));

	char* ptr = (char*)dest;

	while(count--){
		*ptr++ = ch;
	}

	return dest;
}

void* memcpy(void* dest, const void* src, size_t count)
{
	assert((!IsEqual(dest, nullptr)) && (!IsEqual(src, nullptr)));

	char* lptr = (char*)dest;
	char* rptr = (char*)src;

	if(lptr <= rptr){

		while(count--){
			*lptr++ = *rptr++;
		}

	}else{
		lptr += count - 1;
		rptr += count - 1;

		while(count--){
			*lptr-- = *rptr--;
		}
	}

	return dest;
}

void* memchr(const void* str, int ch, size_t count)
{
	assert((!IsEqual(str, nullptr)));
	
	char* ptr = (char*)str;

	while((count--) && (!IsEqual(*ptr, ch))){
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

char* strndup(const char* src, size_t strlen)
{
	assert(!IsEqual(src, nullptr));

	char* ret = malloc(strlen + 1);

	return strncpy(ret, src, strlen + 1);
}

static int* make_pmt(const char* str)
{
	int len = strlen(str);

	int* ret = (int*)malloc(sizeof(int) * len);

	int ll = 0;

	ret[0] = ll;

	for(int i = 1; !IsEqual(i, len); ++i)
	{
		while((!IsEqual(ll, 0)) && (!IsEqual(str[i], str[ll])))
		{
			ll = ret[ll - 1];
		}

		if(IsEqual(str[i], str[ll]))
		{
			++ll;
		}

		ret[i] = ll;
	}

	return ret;
}

int strfind(const char* src, const char* findStr)
{
	assert((!IsEqual(src, nullptr)) && (!IsEqual(findStr, nullptr)));

	int sLen = strlen(src), fLen = strlen(findStr), i = 0, j = 0;

	if((fLen <= 0) || (fLen > sLen))
	{
		return -1;
	}

	int* pmt = make_pmt(findStr);

	for(i = 0, j = 0; (!IsEqual(i, fLen)) && (!IsEqual(j, sLen)); ++j)
	{
		while((!IsEqual(i, 0) && (!IsEqual(src[j], findStr[i]))))
		{
			i = pmt[i - 1];
		}

		if(IsEqual(src[j], findStr[i]))
		{
			++i;
		}
	}

	free(pmt);

	return IsEqual(i, fLen) ? j - fLen : -1;
}
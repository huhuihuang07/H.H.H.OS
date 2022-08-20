#include <string.h>
#include <memory.h>
#include <assert.h>
#include <utility.h>

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

	while((*lptr++ = *rptr++) && strlen)
	{
		strlen--;
	}

	if (strlen)
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

	int* ptr = (int*)dest;

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
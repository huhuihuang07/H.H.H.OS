#include "string.h"

char* strcpy(char* dest, const char* src)
{
	char* lptr = dest;
	char* rptr = (char*)src;

	while(*lptr++ = *rptr++);

	return dest;
}

char* strcat(char* dest, const char* src)
{
	char* ptr = dest + strlen(dest);

	strcpy(ptr, src);

	return dest;
}

size_t strlen(const char* str)
{
	char* ptr = (char*)str;

	while(*ptr++);

	return (ptr - str) - 1;
}

int strcmp(const char* lhs, const char* rhs)
{
	while((*lhs == *rhs) && (*lhs) && (*rhs)){
		lhs++;
		rhs++;
	}

	return *lhs < *rhs ? -1 : *lhs > *rhs;
}

char* strchr(const char* str, int ch)
{
	char* ptr = (char*)str;

	while((ch != *ptr) && (*ptr)){
		++ptr;
	}

	return ch == *ptr ? ptr : nullptr;
}

char* strrchr(const char* str, int ch)
{
	char* ptr = (char*)str + strlen(str);

	while((ch != *ptr) && (str != ptr)){
		--ptr;
	}

	return ch == *ptr ? ptr : nullptr;
}

int memcmp(const void* lhs, const void* rhs, size_t count)
{
	char* lptr = (char*)lhs;
	char* rptr = (char*)rhs;

	while((count --) && (*lptr++ = *rptr++));

	return *lptr < *rptr ? -1 : *lptr > *rptr;
}

void* memset(void* dest, int ch, size_t count)
{
	char* ptr = (char*)dest;

	while(count--){
		*ptr++ = ch;
	}

	return dest;
}

void* memcpy(void* dest, const void* src, size_t count)
{
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
	char* ptr = (char*)str;

	while((count--) && (ch != *ptr)){
		++ptr;
	}

	return ch == *ptr ? ptr : nullptr;
}
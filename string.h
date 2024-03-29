#pragma once

#include "type.h"

char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);
size_t strlen(const char* str);
int32_t strcmp(const char* lhs, const char* rhs);
int32_t strncmp(const char* lhs, const char* rhs, size_t n);
char* strchr(const char* str, int32_t ch);
char* strrchr(const char* str, int32_t ch);
int32_t strfind(const char* src, const char* needle);
char* strstr(const char* src, const char* needle);

char* strdup(const char* src);
char* strndup(const char* src, size_t n);

int32_t memcmp(const void* lhs, const void* rhs, size_t count);
void* memset(void* dest, int32_t ch, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
void* memchr(const void* str, int32_t ch, size_t count);
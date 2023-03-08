#pragma once

#ifndef nullptr
#define nullptr (void*)0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef _packed
#define _packed __attribute__((packed))
#endif

#ifndef EOF
#define EOF -1 // End of file
#endif

#ifndef EOS
#define EOS '\0' // End of string
#endif

#ifndef __cplusplus
typedef enum {
    false = 0,
    true  = !false,
} bool;
#endif

typedef enum {
    Disable = 0,
    Enable  = !Disable,
} state_t;

typedef void (*pFunc_t)();

typedef signed char int8_t;

typedef signed short int16_t;

typedef signed int int32_t;

typedef signed long long int64_t;

typedef unsigned char uint8_t;

typedef unsigned short uint16_t;

typedef unsigned int uint32_t;

typedef unsigned long long uint64_t;

typedef unsigned int size_t;

typedef int32_t pid_t;

typedef char* va_list;

#ifndef _INTSIZEOF
#define _INTSIZEOF(tp) ((sizeof(tp) + sizeof(int) - 1) & ~(sizeof(int) - 1))
#endif

#ifndef va_start
#define va_start(p_list, arg) (p_list = (va_list)&arg + _INTSIZEOF(arg))
#endif

#ifndef va_arg
#define va_arg(p_list, tp) (*(tp*)((p_list += _INTSIZEOF(tp)) - _INTSIZEOF(tp)))
#endif

#ifndef va_end
#define va_end(p_list) (p_list = (va_list)0)
#endif
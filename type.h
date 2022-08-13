#ifndef TYPE_H
#define TYPE_H

#ifndef nullptr
#define nullptr (void*)0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef _packed
#define _packed __attribute__ ((packed))
#endif

#ifndef EOF
#define EOF -1 // End of file
#endif

#ifndef EOS
#define EOS '\0' // End of string
#endif

#ifndef __cplusplus
typedef enum
{
	false = 0,
	true = !false,
}bool;
#endif

typedef void (* pFunc)();

typedef signed char int8;

typedef signed short int16;

typedef signed int int32;

typedef signed long long int64;

typedef unsigned char u8;

typedef unsigned short u16;

typedef unsigned int u32;

typedef unsigned long long u64;

typedef unsigned int size_t;

typedef char* va_list;

#ifndef _INTSIZEOF
#define _INTSIZEOF(tp)   ( (sizeof(tp) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
#endif

#ifndef va_start
#define va_start(p_list,arg)  ( p_list = (va_list)&arg + _INTSIZEOF(arg) )
#endif

#ifndef va_arg
#define va_arg(p_list,tp)    ( *(tp *)((p_list += _INTSIZEOF(tp)) - _INTSIZEOF(tp)) )
#endif

#ifndef va_end
#define va_end(p_list)      ( p_list = (va_list)0 )
#endif

#endif //!TYPE_H
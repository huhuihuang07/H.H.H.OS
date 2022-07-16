#ifndef SCREEN_H
#define SCREEN_H

#include "type.h"

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH  80
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 25
#endif

#ifndef CRT_ADDR_REG 
#define CRT_ADDR_REG 0x3d4
#endif

#ifndef CRT_DATA_REG
#define CRT_DATA_REG 0x3d5
#endif

#ifndef CRT_CURSOR_H
#define CRT_CURSOR_H 0x0e
#endif 

#ifndef CRT_CURSOR_L
#define CRT_CURSOR_L 0x0f
#endif 

typedef enum{
	SCREEN_GRAY   = 0x07,
	SCREEN_BLUE   = 0x09,
	SCREEN_GREEN  = 0x0a,
	SCREEN_RED    = 0x0c,
	SCREEN_YELLOW = 0x0e,
	SCREEN_WHITE  = 0x0f
}PrintColor;

typedef enum{
	SCREEN_BINARY  = 0x02,
	SCREEN_TERNARY = 0x03,
	SCREEN_OCTAL   = 0x08,
	SCREEN_DECIMAL = 0x0a,
	SCREEN_HEXADEC = 0x10
}PrintRadix;

typedef struct{
	u8 width;
	u8 height;
	PrintColor color;
}PrintInfo;

bool ClearScreen();
bool SetPrintPos(u8 w, u8 h);
bool SetPrintColor(PrintColor color);
u16 PrintString(const char* buffer);
u16 PrintIntDec(int n);
u16 PrintIntHex(int n);
u16 PrintAddress(u32 n);

u8 putchar(char c);
u16 printk(const char* format, va_list v_arg);
u16 printf(const char* format, ...);

extern void printChar(u32 position, u16 value);

#endif //!SCREEN_H
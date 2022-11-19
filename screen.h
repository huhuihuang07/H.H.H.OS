#ifndef SCREEN_H
#define SCREEN_H

#include "type.h"

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH  80
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 25
#endif

#ifndef SCREEN_MEM_BASE
#define SCREEN_MEM_BASE 0xB8000
#endif

#ifndef SCREEN_MEM_SIZE
#define SCREEN_MEM_SIZE 0x4000
#endif

#ifndef CRT_ADDR_REG 
#define CRT_ADDR_REG 0x03d4
#endif

#ifndef CRT_DATA_REG
#define CRT_DATA_REG 0x03d5
#endif

#ifndef CRT_START_ADDR_H
#define CRT_START_ADDR_H 0x0c
#endif

#ifndef CRT_START_ADDR_L
#define CRT_START_ADDR_L 0x0d
#endif

#ifndef CRT_CURSOR_H
#define CRT_CURSOR_H 0x0e
#endif 

#ifndef CRT_CURSOR_L
#define CRT_CURSOR_L 0x0f
#endif 

typedef enum{
	SCREEN_INVALID       = -1,   // 无效
	SCREEN_BLACK         = 0x00, // 黑色
	SCREEN_BLUE          = 0x01, // 蓝色
	SCREEN_GREEN         = 0x02, // 绿色
	SCREEN_CYAN          = 0x03, // 青色
	SCREEN_RED           = 0x04, // 红色
	SCREEN_MAGENTA       = 0x05, // 品红
	SCREEN_BROWN         = 0x06, // 棕色
	SCREEN_LIGHT_GREY    = 0x07, // 灰白
	SCREEN_DARK_GREY     = 0x08, // 深灰
	SCREEN_LIGHT_BLUE    = 0x09, // 淡蓝
	SCREEN_LIGHT_GREEN   = 0x0a, // 淡绿
	SCERRN_LIGHT_CYAN    = 0x0b, // 淡青
	SCREEN_LIGHT_RED     = 0x0c, // 淡红
	SCREEN_LIGHT_MAGENTA = 0x0d, // 淡品红
	SCREEN_YELLOW        = 0x0e, // 黄色
	SCREEN_WHITE         = 0x0f, // 白色
	SCREEN_NUM
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

typedef enum{
	SysCall_Screen_Printf = 0,
}SysCall_SCREEN_CMD;

void InitScreen();

bool ClearScreen();
bool SetPrintPos(u8 w, u8 h);
bool SetPrintColor(PrintColor color);
PrintColor GetPrintColor();
u16 PrintString(const char* buffer);
u16 PrintIntDec(int n);
u16 PrintIntHex(int n);
u16 PrintAddress(u32 n);

u8 putchar(const char c);
u16 printk(const char* format, va_list v_arg);
u16 print(const char* format, ...);

u32 ScreenCallHandler(u32 cmd, u32 param1, u32 param2);

extern int printChar(const u32 position, const u16 value);

#endif //!SCREEN_H
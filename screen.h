#ifndef SCREEN_H
#define SCREEN_H

#include "type.h"

#define SCREEN_WIDTH  80
#define SCREEN_HEIGHT 25

typedef enum{
	SCREEN_GRAY   = 0x07,
	SCREEN_BLUE   = 0x09,
	SCREEN_GREEN  = 0x0a,
	SCREEN_RED    = 0x0c,
	SCREEN_YELLOW = 0x0e,
	SCREEN_WHITE  = 0x0f
}PrintColor;

typedef struct{
	u16 width;
	u16 height;
	PrintColor color;
}PrintInfo;

bool ClearScreen();
bool SetPrintPos(u16 w, u16 h);
bool SetPrintColor(PrintColor color);
bool PrintChar(char c);
u16 PrintString(const char* buffer);
u16 PrintIntDec(int n);
u16 PrintIntHex(u32 n);

#endif //!SCREEN_H
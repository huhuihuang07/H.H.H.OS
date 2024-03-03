#pragma once

#include "type.h"

#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 80u
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 25u
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

typedef enum {
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
} PrintColor_t;

typedef enum {
    SCREEN_BINARY  = 0x02,
    SCREEN_TERNARY = 0x03,
    SCREEN_OCTAL   = 0x08,
    SCREEN_DECIMAL = 0x0a,
    SCREEN_HEXADEC = 0x10
} PrintRadix_t;

typedef struct
{
    uint8_t width;
    uint8_t height;
    PrintColor_t color;
} PrintInfo_t;

typedef enum {
    SysCall_Screen_Printf = 0,
} SysCall_SCREEN_CMD_t;

void ScreenInit();

bool ClearScreen();
bool SetPrintPos(uint8_t w, uint8_t h);
bool SetPrintColor(PrintColor_t color);
PrintColor_t GetPrintColor();
uint32_t PrintString(const char* buffer);
uint32_t PrintIntDec(int32_t n);
uint32_t PrintIntHex(int32_t n);
uint32_t PrintAddress(uint32_t n);

void PrintLogo();

uint8_t putchar(const char c);
uint32_t printk(const char* format, va_list v_arg);
uint32_t print(const char* format, ...);

uint32_t ScreenCallHandler(uint32_t cmd, uint32_t param1, uint32_t param2);

extern int32_t printChar(const uint32_t position, const uint16_t value);
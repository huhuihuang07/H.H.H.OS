#pragma once

#include "type.h"

typedef enum {
    KeyRelease = 0,
    KeyPress   = 0x1000000
} KeyState;

extern void KeyBoardInit();

void KeyBoardModuleInit();
void PutScanCode(uint8_t scancode);
uint32_t FetchKeyCode();
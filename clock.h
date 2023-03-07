#pragma once

#include "type.h"

// 可编程定时计数器 (Programmable Interval Timer PIT)

// 定时器0 端口
#ifndef PIT_CHAN0_REG
#define PIT_CHAN0_REG 0X40
#endif

// 定时器1 端口
#ifndef PIT_CHAN1_REG
#define PIT_CHAN1_REG 0X41
#endif

// 定时器2 端口
#ifndef PIT_CHAN2_REG
#define PIT_CHAN2_REG 0X42
#endif

// 控制字寄存器 端口
#ifndef PIT_CTRL_REG
#define PIT_CTRL_REG 0X43
#endif

#ifndef HZ
#define HZ 100
#endif

u32 jiffy = 1000 / HZ;

void ClockInit();

extern void TimerInit();
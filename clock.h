#ifndef CLOCK_H
#define CLOCK_H

#include "type.h"

// 可编程定时计数器 (Programmable Interval Timer PIT) 
#ifndef PIT_CHAN0_REG
#define PIT_CHAN0_REG 0X40
#endif

#ifndef PIT_CHAN2_REG
#define PIT_CHAN2_REG 0X42
#endif

#ifndef PIT_CTRL_REG
#define PIT_CTRL_REG 0X43
#endif

#ifndef HZ
#define HZ 100
#endif

u32 jiffy = 1000 / HZ;

void ClockInit();

extern void TimerInit();

#endif //!CLOCK_H
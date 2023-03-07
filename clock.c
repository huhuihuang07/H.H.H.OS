#include "clock.h"
#include "io.h"

static void PitInit()
{
    static const uint32_t OSCILLATOR = 1193182;

    uint32_t CLOCK_COUNTER = OSCILLATOR / HZ;

    outb(PIT_CTRL_REG, 0b00110100);
    outb(PIT_CHAN0_REG, CLOCK_COUNTER & 0xff);
    outb(PIT_CHAN0_REG, (CLOCK_COUNTER >> 8) & 0xff);
}

void ClockInit()
{
    PitInit();
    TimerInit();
}
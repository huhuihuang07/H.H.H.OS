#include "assert.h"
#include "screen.h"

void AssertionFailure(const char* exp, const char* file, uint32_t line)
{
    ClearScreen();

    SetPrintPos(0, 0);

    print(
        "--> assert(%s) failed!!!\n"
        "--> file : %s\n"
        "--> line : %d\n",
        exp, file, line);

    while (true)
        ;
}
#include "assert.h"
#include "screen.h"

void assertionFailure(const char *exp, const char *file, u32 line)
{
	ClearScreen();

	SetPrintPos(0, 0);

	printf(
		"--> assert(%s) failed!!!\n"
		"--> file : %s\n"
		"--> line : %d\n",
		exp, file, line);

	while(true);
}
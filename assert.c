#include <assert.h>
#include <screen.h>

void assertionFailure(const char *exp, const char *file, int line)
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
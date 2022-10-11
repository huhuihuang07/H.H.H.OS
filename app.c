#include "app.h"
#include "syscall.h"

void TaskA()
{
	static u32 i = 0;

	while(true){

		// SetPrintPos(0, 2);

		printf("a");

		// putchar('a' + i);

		i = (i + 1) % 26;

		Sleep(1000);
	}
}

void TaskB()
{
	static u32 i = 0;

	while(true){

		// SetPrintPos(0, 3);

		printf("b");

		// putchar('a' + i);

		i = (i + 1) % 26;

		Sleep(100);
	}
}

void TaskC()
{
	static u32 i = 0;

	while(true){

		// SetPrintPos(0, 4);

		printf("c");

		// putchar('a' + i);

		i = (i + 1) % 26;

		Sleep(1000);
	}
}

void TaskD()
{
	static u32 i = 0;

	while(true){

		// SetPrintPos(0, 5);

		printf("d");

		// putchar('a' + i);

		i = (i + 1) % 26;

		Sleep(100000);
	}
}

void AMain()
{
	if(IsEqual(RegisterApp("TaskA", TaskA, 255), true))
	{
		RegisterApp("TaskB", TaskB, 255);
		RegisterApp("TaskC", TaskC, 255);
		RegisterApp("TaskD", TaskD, 255);
	}
}
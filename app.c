#include "app.h"
#include "syscall.h"
#include "screen.h"

void TaskA()
{
	static u32 i = 0;

	while(true){

		SetPrintPos(0, 2);

		printf("This is TaskA : ");

		putchar('a' + i);

		i = (i + 1) % 26;

		Sleep(1000);
	}
}

void TaskB()
{
	static u32 i = 0;

	while(true){

		SetPrintPos(0, 3);

		printf("This is TaskB : ");

		putchar('a' + i);

		i = (i + 1) % 26;

		Sleep(100);
	}
}

void TaskC()
{
	static u32 i = 0;

	while(true){

		SetPrintPos(0, 4);

		printf("This is TaskC : ");

		putchar('a' + i);

		i = (i + 1) % 26;

		Sleep(1000);
	}
}

void TaskD()
{
	static u32 i = 0;

	while(true){

		SetPrintPos(0, 5);

		printf("This is TaskD : ");

		putchar('a' + i);

		i = (i + 1) % 26;

		Sleep(100000);
	}
}

void AMain()
{

	int* p = (int*)(0x12345678);

	*p = 65530;

	// if(IsEqual(RegisterApp("TaskA", TaskA, 255), true))
	// {
	// 	RegisterApp("TaskB", TaskB, 255);
	// 	RegisterApp("TaskC", TaskC, 255);
	// 	RegisterApp("TaskD", TaskD, 255);
	// }
}
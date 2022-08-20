#include <app.h>
#include <syscall.h>
#include <screen.h>

void TaskA()
{
	static u32 i = 0;

	while(true){

		SetPrintPos(0, 2);

		printf("This is TaskA : ");

		putchar('a' + i);

		i = (i + 1) % 26;

		Delay(1);
	}
}

void TaskB()
{
	static u32 i = 0;

	while(i < 10){

		SetPrintPos(0, 3);

		printf("This is TaskB : ");

		putchar('a' + i);

		i = (i + 1) % 26;

		Delay(1);
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

		Delay(1);
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

		Delay(1);
	}
}

void AMain()
{
	RegisterApp("TaskA", TaskA, 250);
	RegisterApp("TaskB", TaskB, 240);
	RegisterApp("TaskC", TaskC, 255);
	RegisterApp("TaskD", TaskD, 255);
}
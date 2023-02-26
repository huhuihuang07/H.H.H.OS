#include "ihandler.h"
#include "syscall.h"
#include "screen.h"

static char *messages[] = {
	"#DE Divide Error\0",
	"#DB RESERVED\0",
	"--  NMI Interrupt\0",
	"#BP Breakpoint\0",
	"#OF Overflow\0",
	"#BR BOUND Range Exceeded\0",
	"#UD Invalid Opcode (Undefined Opcode)\0",
	"#NM Device Not Available (No Math Coprocessor)\0",
	"#DF Double Fault\0",
	"    Coprocessor Segment Overrun (reserved)\0",
	"#TS Invalid TSS\0",
	"#NP Segment Not Present\0",
	"#SS Stack-Segment Fault\0",
	"#GP General Protection\0",
	"#PF Page Fault\0",
	"--  (Intel reserved. Do not use.)\0",
	"#MF x87 FPU Floating-Point Error (Math Fault)\0",
	"#AC Alignment Check\0",
	"#MC Machine Check\0",
	"#XF SIMD Floating-Point Exception\0",
	"#VE Virtualization Exception\0",
	"#CP Control Protection Exception\0",
};

void TimerHandler(u32 vector, u32 error_code, PrivilegeLevel privilegeLevel)
{
	if (IsEqual(privilegeLevel, User))
	{
		TaskCallHandler(1, 0, 0);
	}

	SendEOI(MASTER_EOI_PORT);
}

void PageFaultHandler(u32 vector, u32 error_code, PrivilegeLevel privilegeLevel)
{
	PageFault(error_code);
}

void DefaultInterruptHandler(u32 vector, u32 error_code, PrivilegeLevel privilegeLevel)
{
	print("Interrupt : %p %p", vector, error_code);

	if (vector < ArraySize(messages))
	{
		print("message : %s", messages[vector]);
	}
}

u32 SysCallHandler(u32 type, u32 cmd, u32 param1, u32 param2)
{
	u32 ret = 0;

	switch (type)
	{
	case SysCall_Task:
	{
		ret = TaskCallHandler(cmd, param1, param2);
		break;
	}
	case SysCall_Screen:
	{
		ret = ScreenCallHandler(cmd, param1, param2);
		break;
	}
	default:
		break;
	}

	return ret;
}
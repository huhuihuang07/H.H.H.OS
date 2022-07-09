#include "screen.h"

static PrintInfo printInfo = {0, 0, SCREEN_GRAY};

static bool SetCursorPos(u16 w, u16 h)
{
	bool ret = (w < SCREEN_WIDTH) && (h < SCREEN_HEIGHT);

	if(ret){
		u16 bx = h * SCREEN_WIDTH + w;

		asm volatile(
			"movw %0,      %%bx\n"
			"\n"
			"movw $0x03d4, %%dx\n"
			"movb $0x0e,   %%al\n"
			"outb %%al,    %%dx\n"
			"movw $0x03d5, %%dx\n"
			"movb %%bh,    %%al\n"
			"outb %%al,    %%dx\n"
			"\n"
			"movw $0x03d4, %%dx\n"
			"movb $0x0f,   %%al\n"
			"outb %%al,    %%dx\n"
			"movw $0x03d5, %%dx\n"
			"movb %%bl,    %%al\n"
			"outb %%al,    %%dx\n"
			"\n"
			:
			: "r"(bx)
			: "ax", "bx", "dx"
		);
	}

	return ret;
}

bool ClearScreen()
{
	u32 ret = 0;

	for(u16 i = 0; i != SCREEN_WIDTH; ++i){
		for(u16 j = 0; j != SCREEN_HEIGHT; ++j){
			if(SetPrintPos(i, j) && PrintChar(' ')){
				ret++;
			}
		}
	}

	return ret == (SCREEN_WIDTH * SCREEN_HEIGHT);
}

bool SetPrintPos(u16 w, u16 h)
{
	bool ret = (w < SCREEN_WIDTH) && (h < SCREEN_HEIGHT);

	if(ret){
		printInfo.width = w;
		printInfo.height = h;

		SetCursorPos(w, h);
	}

	return ret;
}

bool SetPrintColor(PrintColor color)
{
	bool ret = true;

	printInfo.color = color;

	return ret;
}

bool PrintChar(char c)
{
	bool ret = ('\n' == c) || ('\t' == c);

	if(ret){

		if('\n' == c){

			printInfo.width = 0;

			printInfo.height += 1;

			if(SCREEN_HEIGHT == printInfo.height){
				printInfo.height = 0;
			}
		}else{
			ret = PrintChar(' ');
		}

	}else{
		ret = (printInfo.width < SCREEN_WIDTH) && (printInfo.height < SCREEN_HEIGHT);

		if(ret){
			u32 edi = (printInfo.height * SCREEN_WIDTH + printInfo.width) << 1;
			u8 ah = printInfo.color;
			char al = c;

			asm volatile (
				"movl %0,          %%edi\n"
				"movb %1,          %%ah \n"
				"movb %2,          %%al \n"
				"movw %%ax, %%gs:(%%edi)\n"
				: 
				: "r"(edi), "r"(ah), "r"(al)
				: "ax", "edi"
			);

			printInfo.width += 1;

			if(SCREEN_WIDTH == printInfo.width){

				printInfo.width = 0;

				printInfo.height += 1;

				if(SCREEN_HEIGHT == printInfo.height){
					printInfo.height = 0;
				}
			}
		}
	}

	if(ret){
		SetCursorPos(printInfo.width, printInfo.height);
	}

	return ret;
}

u16 PrintString(const char* buffer)
{
	u16 ret = 0;

	if(nullptr != buffer){

		while('\0' != buffer[ret]){

			if(PrintChar(buffer[ret])){
				ret++;
			}else{
				break;
			}
		}
	}

	return ret;
}

u16 PrintIntDec(int n)
{
	u16 ret = 0;

	return ret;
}

u16 PrintIntHex(u32 n)
{
	u16 ret = 0;

	return ret;
}
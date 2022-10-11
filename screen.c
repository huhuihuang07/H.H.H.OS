#include "screen.h"
#include "const.h"
#include "io.h"
#include "string.h"
#include "utility.h"
#include "assert.h"

static PrintInfo printInfo = {0, 0, SCREEN_GRAY};

static u16 SCREEN_ERASER = (SCREEN_GRAY << 8) | ' ';

static u16 SCREEN_POS = 0;

// 设置当前光标位置
static bool SetCursorPos(u8 w, u8 h)
{
	bool ret = (w < SCREEN_WIDTH) && (h < SCREEN_HEIGHT);

	if(ret){
		u32 bx = (h + SCREEN_POS) * SCREEN_WIDTH + w;

		outb(CRT_ADDR_REG, CRT_CURSOR_H);

		outb(CRT_DATA_REG, (bx >> 8) & 0xff);

		outb(CRT_ADDR_REG, CRT_CURSOR_L);
		
		outb(CRT_DATA_REG, bx & 0xff);
	}

	return ret;
}

// 设置当前文本模式显存开始位置
static bool SetScreenPos(u8 w, u8 h)
{
	bool ret = true;

	if(ret){
		u32 bx = h * SCREEN_WIDTH + w;

		outb(CRT_ADDR_REG, CRT_START_ADDR_H);

		outb(CRT_DATA_REG, (bx >> 8) & 0xff);

		outb(CRT_ADDR_REG, CRT_START_ADDR_L);
		
		outb(CRT_DATA_REG, bx & 0xff);
	}

	return ret;
}

// 向上滚屏
static void scrollUp()
{
	SCREEN_POS += 1;

	printInfo.height -= 1;

	u32 address = (SCREEN_POS + SCREEN_HEIGHT) * SCREEN_WIDTH << 1;

	if(address < SCREEN_MEM_SIZE)
	{
		u16* base = (u16*)(SCREEN_MEM_BASE + address);

		for(int i = 0; !IsEqual(i, SCREEN_WIDTH); ++i)
		{
			base[i] = SCREEN_ERASER;
		}
	}else{
		memcpy((void*)(SCREEN_MEM_BASE), (void*)(SCREEN_MEM_BASE + (SCREEN_POS * SCREEN_WIDTH << 1)), (SCREEN_HEIGHT - 1) * SCREEN_WIDTH << 1);

		u16* base = (u16*)(SCREEN_MEM_BASE + ((SCREEN_HEIGHT - 1) * SCREEN_WIDTH << 1));

		for(int i = 0; !IsEqual(i, SCREEN_WIDTH); ++i)
		{
			base[i] = SCREEN_ERASER;
		}

		SCREEN_POS = 0;
	}

	SetScreenPos(0, SCREEN_POS);
}

static u16 PrintIntRadix(int n, PrintRadix radix)
{
	u16 ret = 0;

	if((SCREEN_BINARY <= radix) && (radix <= SCREEN_HEXADEC)){

		if(n < radix){
			ret += putchar("0123456789abcdef"[n]);
		}else{
			ret += PrintIntRadix(n / radix, radix) + PrintIntRadix(n % radix, radix);
		}

	}

	return ret;
}

bool ClearScreen()
{
	u16 ret = 0;

	SetPrintPos(0, 0);

	for(u8 i = 0; !IsEqual(i, SCREEN_WIDTH); ++i){
		for(u8 j = 0; !IsEqual(j, SCREEN_HEIGHT); ++j){
			if(putchar(SCREEN_ERASER)){
				ret++;
			}
		}
	}

	return IsEqual(ret, (SCREEN_WIDTH * SCREEN_HEIGHT));
}

void InitScreen()
{
	assert(ClearScreen());

	SetScreenPos(0, SCREEN_POS = 0);

	SetPrintPos(0, 0);
}

bool SetPrintPos(u8 w, u8 h)
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

	switch(color){
		case SCREEN_GRAY   : { printInfo.color = color; break; }
		case SCREEN_BLUE   : { printInfo.color = color; break; }
		case SCREEN_GREEN  : { printInfo.color = color; break; }
		case SCREEN_RED    : { printInfo.color = color; break; }
		case SCREEN_YELLOW : { printInfo.color = color; break; }
		case SCREEN_WHITE  : { printInfo.color = color; break; }
		default : ret = false;
	}

	return ret;
}

u8 putchar(char c)
{
	bool ret = IsEqual(c, '\n') || IsEqual(c, '\t');

	if(ret){

		if(IsEqual(c, '\n')){

			printInfo.width = 0;

			printInfo.height += 1;

			if(IsEqual(printInfo.height, SCREEN_HEIGHT)){
				scrollUp();
			}
		}else{
			ret = putchar(SCREEN_ERASER);
		}

	}else{

		if(ret = (printInfo.width < SCREEN_WIDTH) && (printInfo.height < SCREEN_HEIGHT)){

			u32 edi = (printInfo.height + SCREEN_POS) * SCREEN_WIDTH + printInfo.width;

			u16 ax = (printInfo.color << 8) | c;

			printInfo.width += printChar(edi, ax);

			if(IsEqual(printInfo.width, SCREEN_WIDTH)){

				printInfo.width = 0;

				printInfo.height += 1;

				if(IsEqual(printInfo.height, SCREEN_HEIGHT)){
					scrollUp();
				}
			}
		}
	}

	if(ret){
		SetCursorPos(printInfo.width, printInfo.height);
	}

	return ret ? 1 : 0;
}

u16 PrintString(const char* buffer)
{
	assert(!IsEqual(buffer, nullptr));

	u16 ret = 0;

	while(buffer[ret])
	{
		if(putchar(buffer[ret]))
		{
			ret++;
		}else{
			break;
		}
	}

	return ret;
}

u16 PrintIntDec(int n)
{
	return n < 0 ? (putchar('-') + PrintIntDec(-n)) : (PrintIntRadix(n, SCREEN_DECIMAL));
}

u16 PrintIntHex(int n)
{
	return (PrintString("0x") + PrintIntRadix(n, SCREEN_HEXADEC));
}

u16 PrintAddress(u32 n)
{
	char buffer[] = {'0', 'x', '0', '0', '0', '0', '0', '0', '0', '0', EOS};

	for(u8 i = 9; (!IsEqual(i, 1)) && (!IsEqual(n, 0)); --i){

		buffer[i] = "0123456789abcdef"[n % SCREEN_HEXADEC];

		n /= SCREEN_HEXADEC;
	}

	return PrintString(buffer);
}

u16 printk(const char* format, va_list v_arg)
{
	assert(!IsEqual(format, nullptr));

	u16 ret = 0;

	bool flag = false;

	for(u16 i = 0; !IsEqual(format[i], EOS); ++i)
	{
		if((IsEqual(flag, false)) && (!IsEqual(format[i], '%')))
		{
			ret += putchar(format[i]);
		}else{
			if(flag)
			{
				switch (format[i])
				{
					case 'b' : {ret += PrintIntRadix(va_arg(v_arg, int), SCREEN_BINARY); break;}
					case 'd' : {ret += PrintIntDec(va_arg(v_arg, int)); break;}
					case 'o' : {ret += PrintIntRadix(va_arg(v_arg, int), SCREEN_OCTAL); break;}
					case 'x' : {ret += PrintIntHex(va_arg(v_arg, int)); break;}
					case 'X' : {ret += PrintIntHex(va_arg(v_arg, int)); break;}
					case 'u' : {ret += PrintIntRadix(va_arg(v_arg, int), SCREEN_DECIMAL); break;}
					case 'c' : {ret += putchar(va_arg(v_arg, char)); break;}
					case 's' : {ret += PrintString(va_arg(v_arg, const char*)); break;}
					case 'p' : {ret += PrintAddress(va_arg(v_arg, u32)); break;}
					default  : {ret += putchar(format[i]);}
				}
				flag = false;
			}else{
				flag = true;
			}
		}
	}

	return ret;
}

u16 print(const char* format, ...)
{
	assert(!IsEqual(format, nullptr));

	u16 ret = 0;

	va_list v_arg;

	va_start(v_arg, format);

	ret = printk(format, v_arg);

	va_end(v_arg);

	return ret;
}

u32 ScreenCallHandler(u32 cmd, u32 param1, u32 param2)
{
	u32 ret = 0;

	switch(cmd){
		case SysCall_Screen_Printf : {
			ret = printk((const char*)(param1), (va_list)(param2));
			break;
		}
		default : 
			break;
	}

	return ret;
}
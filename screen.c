#include "screen.h"
#include "io.h"

static PrintInfo printInfo = {0, 0, SCREEN_GRAY};

static bool SetCursorPos(u8 w, u8 h)
{
	bool ret = (w < SCREEN_WIDTH) && (h < SCREEN_HEIGHT);

	if(ret){
		u16 bx = h * SCREEN_WIDTH + w;

		outb(CRT_ADDR_REG, CRT_CURSOR_H);

		outb(CRT_DATA_REG, (bx >> 8) & 0xff);

		outb(CRT_ADDR_REG, CRT_CURSOR_L);
		
		outb(CRT_DATA_REG, bx & 0xff);
	}

	return ret;
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

	for(u8 i = 0; i != SCREEN_WIDTH; ++i){
		for(u8 j = 0; j != SCREEN_HEIGHT; ++j){
			if(SetPrintPos(i, j) && putchar(' ')){
				ret++;
			}
		}
	}

	return ret == (SCREEN_WIDTH * SCREEN_HEIGHT);
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
	bool ret = ('\n' == c) || ('\t' == c);

	if(ret){

		if('\n' == c){

			printInfo.width = 0;

			printInfo.height += 1;

			if(SCREEN_HEIGHT == printInfo.height){
				printInfo.height = 0;
			}
		}else{
			ret = putchar(' ');
		}

	}else{

		if(ret = (printInfo.width < SCREEN_WIDTH) && (printInfo.height < SCREEN_HEIGHT)){
			u32 edi = printInfo.height * SCREEN_WIDTH + printInfo.width;
			u16 ax = (printInfo.color << 8) | c;

			printChar(edi, ax);

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

	return ret ? 1 : 0;
}

u16 PrintString(const char* buffer)
{
	u16 ret = 0;

	if(nullptr != buffer){

		while('\0' != buffer[ret]){

			if(putchar(buffer[ret])){
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
	return n < 0 ? (putchar('-') + PrintIntDec(-n)) : (PrintIntRadix(n, SCREEN_DECIMAL));
}

u16 PrintIntHex(int n)
{
	return (PrintString("0x") + PrintIntRadix(n, SCREEN_HEXADEC));
}

u16 PrintAddress(u32 n){
	char buffer[] = {'0', 'x', '0', '0', '0', '0', '0', '0', '0', '0', '\0'};

	for(u8 i = 9; (1 != i) && (0 != n); --i){

		buffer[i] = "0123456789abcdef"[n % SCREEN_HEXADEC];

		n /= SCREEN_HEXADEC;
	}

	return PrintString(buffer);
}

u16 printk(const char* format, va_list v_arg)
{
	u16 ret = 0;

	bool flag = false;

	if(nullptr != format){
		for(u16 i = 0; '\0' != format[i]; ++i){

			if((!flag) && ('%' != format[i])){
				ret += putchar(format[i]);
			}else{
				if(flag){
					switch (format[i]){
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
	}

	return ret;
}

u16 printf(const char* format, ...)
{
	u16 ret = 0;

	if(nullptr != format){
		va_list v_arg;

		va_start(v_arg, format);

		ret = printk(format, v_arg);

		va_end(v_arg);
	}

	return ret;
}
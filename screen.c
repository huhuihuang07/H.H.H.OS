#include "screen.h"
#include "const.h"
#include "io.h"
#include "string.h"
#include "utility.h"
#include "assert.h"

static PrintInfo_t printInfo = {0, 0, SCREEN_LIGHT_GREY};

static const uint16_t SCREEN_ERASER = (SCREEN_LIGHT_GREY << 8) | ' ';

static uint16_t SCREEN_POS = 0;

static const char* logo[] = {
    "   ____       _         \n\0",
    "  / __ \\___  (_)_ __   \n\0",
    " / /_/ / _ \\/ /\\ \\ / \n\0",
    " \\____/_//_/_//_\\_\\  \n\0",
};

// 设置当前光标位置
static bool SetCursorPos(uint8_t w, uint8_t h)
{
    bool ret = (w < SCREEN_WIDTH) && (h < SCREEN_HEIGHT);

    if (ret)
    {
        uint32_t bx = (h + SCREEN_POS) * SCREEN_WIDTH + w;

        outb(CRT_ADDR_REG, CRT_CURSOR_H);

        outb(CRT_DATA_REG, (bx >> 8) & 0xff);

        outb(CRT_ADDR_REG, CRT_CURSOR_L);

        outb(CRT_DATA_REG, bx & 0xff);
    }

    return ret;
}

// 设置当前文本模式显存开始位置
static bool SetScreenPos(uint8_t w, uint8_t h)
{
    bool ret = true;

    if (ret)
    {
        uint32_t bx = h * SCREEN_WIDTH + w;

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

    uint32_t address = (SCREEN_POS + SCREEN_HEIGHT) * SCREEN_WIDTH << 1;

    if (address < SCREEN_MEM_SIZE)
    {
        uint16_t* base = (uint16_t*)(SCREEN_MEM_BASE + address);

        for (int32_t i = 0; !IsEqual(i, SCREEN_WIDTH); ++i)
        {
            base[i] = SCREEN_ERASER;
        }
    }
    else
    {
        memcpy((void*)(SCREEN_MEM_BASE), (void*)(SCREEN_MEM_BASE + (SCREEN_POS * SCREEN_WIDTH << 1)), (SCREEN_HEIGHT - 1) * SCREEN_WIDTH << 1);

        uint16_t* base = (uint16_t*)(SCREEN_MEM_BASE + ((SCREEN_HEIGHT - 1) * SCREEN_WIDTH << 1));

        for (int32_t i = 0; !IsEqual(i, SCREEN_WIDTH); ++i)
        {
            base[i] = SCREEN_ERASER;
        }

        SCREEN_POS = 0;
    }

    SetScreenPos(0, SCREEN_POS);
}

static uint16_t PrintIntRadix(int32_t n, PrintRadix_t radix)
{
    uint16_t ret = 0;

    if ((SCREEN_BINARY <= radix) && (radix <= SCREEN_HEXADEC))
    {
        if (n < radix)
        {
            ret += putchar("0123456789abcdef"[n]);
        }
        else
        {
            ret += PrintIntRadix(n / radix, radix) + PrintIntRadix(n % radix, radix);
        }
    }

    return ret;
}

bool ClearScreen()
{
    uint16_t ret = 0;

    SetPrintPos(0, 0);

    for (uint8_t i = 0; !IsEqual(i, SCREEN_WIDTH); ++i)
    {
        for (uint8_t j = 0; !IsEqual(j, SCREEN_HEIGHT); ++j)
        {
            if (putchar(SCREEN_ERASER))
            {
                ret++;
            }
        }
    }

    SetPrintPos(0, 0);

    uint8_t logoLen = ArraySize(logo);

    for (uint8_t i = 0; !IsEqual(i, logoLen); ++i)
    {
        print(logo[i]);
    }

    return IsEqual(ret, (SCREEN_WIDTH * SCREEN_HEIGHT));
}

void InitScreen()
{
    assert(ClearScreen());

    SetScreenPos(0, SCREEN_POS = 0);
}

bool SetPrintPos(uint8_t w, uint8_t h)
{
    bool ret = (w < SCREEN_WIDTH) && (h < SCREEN_HEIGHT);

    if (ret)
    {
        printInfo.width  = w;
        printInfo.height = h;

        SetCursorPos(w, h);
    }

    return ret;
}

bool SetPrintColor(PrintColor_t color)
{
    bool ret = (SCREEN_INVALID < color) && (color < SCREEN_NUM);

    if (ret)
    {
        printInfo.color = color;
    }

    return ret;
}

PrintColor_t GetPrintColor()
{
    return printInfo.color;
}

uint8_t putchar(const char c)
{
    bool ret = IsEqual(c, '\n') || IsEqual(c, '\t');

    if (ret)
    {
        if (IsEqual(c, '\n'))
        {
            printInfo.width = 0;

            printInfo.height += 1;

            if (IsEqual(printInfo.height, SCREEN_HEIGHT))
            {
                scrollUp();
            }
        }
        else
        {
            ret = putchar(SCREEN_ERASER);
        }
    }
    else
    {
        if (ret = (printInfo.width < SCREEN_WIDTH) && (printInfo.height < SCREEN_HEIGHT))
        {
            uint32_t edi = (printInfo.height + SCREEN_POS) * SCREEN_WIDTH + printInfo.width;

            uint16_t ax = (printInfo.color << 8) | c;

            printInfo.width += printChar(edi, ax);

            if (IsEqual(printInfo.width, SCREEN_WIDTH))
            {
                printInfo.width = 0;

                printInfo.height += 1;

                if (IsEqual(printInfo.height, SCREEN_HEIGHT))
                {
                    scrollUp();
                }
            }
        }
    }

    if (ret)
    {
        SetCursorPos(printInfo.width, printInfo.height);
    }

    return ret ? 1 : 0;
}

uint16_t PrintString(const char* buffer)
{
    assert(!IsEqual(buffer, nullptr));

    uint16_t ret = 0;

    while (buffer[ret])
    {
        if (putchar(buffer[ret]))
        {
            ret++;
        }
        else
        {
            break;
        }
    }

    return ret;
}

uint16_t PrintIntDec(int32_t n)
{
    return n < 0 ? (putchar('-') + PrintIntDec(-n)) : (PrintIntRadix(n, SCREEN_DECIMAL));
}

uint16_t PrintIntHex(int32_t n)
{
    return (PrintString("0x") + PrintIntRadix(n, SCREEN_HEXADEC));
}

uint16_t PrintAddress(uint32_t n)
{
    char buffer[] = {'0', 'x', '0', '0', '0', '0', '0', '0', '0', '0', EOS};

    for (uint8_t i = 9; (!IsEqual(i, 1)) && (!IsEqual(n, 0)); --i)
    {
        buffer[i] = "0123456789abcdef"[n % SCREEN_HEXADEC];

        n /= SCREEN_HEXADEC;
    }

    return PrintString(buffer);
}

uint16_t printk(const char* format, va_list v_arg)
{
    assert(!IsEqual(format, nullptr));

    uint16_t ret = 0;

    bool flag = false;

    for (uint16_t i = 0; !IsEqual(format[i], EOS); ++i)
    {
        if ((IsEqual(flag, false)) && (!IsEqual(format[i], '%')))
        {
            ret += putchar(format[i]);
        }
        else
        {
            if (flag)
            {
                switch (format[i])
                {
                    case 'b': {
                        ret += PrintIntRadix(va_arg(v_arg, int32_t), SCREEN_BINARY);
                        break;
                    }
                    case 'd': {
                        ret += PrintIntDec(va_arg(v_arg, int32_t));
                        break;
                    }
                    case 'o': {
                        ret += PrintIntRadix(va_arg(v_arg, int32_t), SCREEN_OCTAL);
                        break;
                    }
                    case 'x': {
                        ret += PrintIntHex(va_arg(v_arg, int32_t));
                        break;
                    }
                    case 'X': {
                        ret += PrintIntHex(va_arg(v_arg, int32_t));
                        break;
                    }
                    case 'u': {
                        ret += PrintIntRadix(va_arg(v_arg, int32_t), SCREEN_DECIMAL);
                        break;
                    }
                    case 'c': {
                        ret += putchar(va_arg(v_arg, int32_t));
                        break;
                    }
                    case 's': {
                        ret += PrintString(va_arg(v_arg, const char*));
                        break;
                    }
                    case 'p': {
                        ret += PrintAddress(va_arg(v_arg, uint32_t));
                        break;
                    }
                    default: {
                        ret += putchar(format[i]);
                    }
                }
                flag = false;
            }
            else
            {
                flag = true;
            }
        }
    }

    return ret;
}

uint16_t print(const char* format, ...)
{
    assert(!IsEqual(format, nullptr));

    uint16_t ret = 0;

    va_list v_arg;

    va_start(v_arg, format);

    ret = printk(format, v_arg);

    va_end(v_arg);

    return ret;
}

uint32_t ScreenCallHandler(uint32_t cmd, uint32_t param1, uint32_t param2)
{
    uint32_t ret = 0;

    switch (cmd)
    {
        case SysCall_Screen_Printf: {
            ret = printk((const char*)(param1), (va_list)(param2));
            break;
        }
        default:
            break;
    }

    return ret;
}

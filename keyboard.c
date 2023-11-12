#include "keyboard.h"
#include "utility.h"
#include "memory.h"
#include "queue.h"

static Queue_t* pKeyCodeQueue = nullptr;

typedef struct
{
    uint8_t ascii1; // on shift code
    uint8_t ascii2; // shift code
    uint8_t scode;  // scan code
    uint8_t kcode;  // key code
} KeyCode_t;

typedef struct
{
    QueueNode_t head;
    uint32_t keycode;
} KeyCodeNode_t;

static const KeyCode_t gKeyMap[] =
    {
  /* 0x00 - none      */
        {   0,    0,    0,    0},
 /* 0x01 - ESC       */
        {   0,    0, 0x01, 0x1B},
 /* 0x02 - '1'       */
        { '1',  '!', 0x02, 0x31},
 /* 0x03 - '2'       */
        { '2',  '@', 0x03, 0x32},
 /* 0x04 - '3'       */
        { '3',  '#', 0x04, 0x33},
 /* 0x05 - '4'       */
        { '4',  '$', 0x05, 0x34},
 /* 0x06 - '5'       */
        { '5',  '%', 0x06, 0x35},
 /* 0x07 - '6'       */
        { '6',  '^', 0x07, 0x36},
 /* 0x08 - '7'       */
        { '7',  '&', 0x08, 0x37},
 /* 0x09 - '8'       */
        { '8',  '*', 0x09, 0x38},
 /* 0x0A - '9'       */
        { '9',  '(', 0x0A, 0x39},
 /* 0x0B - '0'       */
        { '0',  ')', 0x0B, 0x30},
 /* 0x0C - '-'       */
        { '-',  '_', 0x0C, 0xBD},
 /* 0x0D - '='       */
        { '=',  '+', 0x0D, 0xBB},
 /* 0x0E - BS        */
        {   0,    0, 0x0E, 0x08},
 /* 0x0F - TAB       */
        {   0,    0, 0x0F, 0x09},
 /* 0x10 - 'q'       */
        { 'q',  'Q', 0x10, 0x51},
 /* 0x11 - 'w'       */
        { 'w',  'W', 0x11, 0x57},
 /* 0x12 - 'e'       */
        { 'e',  'E', 0x12, 0x45},
 /* 0x13 - 'r'       */
        { 'r',  'R', 0x13, 0x52},
 /* 0x14 - 't'       */
        { 't',  'T', 0x14, 0x54},
 /* 0x15 - 'y'       */
        { 'y',  'Y', 0x15, 0x59},
 /* 0x16 - 'u'       */
        { 'u',  'U', 0x16, 0x55},
 /* 0x17 - 'i'       */
        { 'i',  'I', 0x17, 0x49},
 /* 0x18 - 'o'       */
        { 'o',  'O', 0x18, 0x4F},
 /* 0x19 - 'p'       */
        { 'p',  'P', 0x19, 0x50},
 /* 0x1A - '['       */
        { '[',  '{', 0x1A, 0xDB},
 /* 0x1B - ']'       */
        { ']',  '}', 0x1B, 0xDD},
 /* 0x1C - CR/LF     */
        {   0,    0, 0x1C, 0x0D},
 /* 0x1D - l. Ctrl   */
        {   0,    0, 0x1D, 0x11},
 /* 0x1E - 'a'       */
        { 'a',  'A', 0x1E, 0x41},
 /* 0x1F - 's'       */
        { 's',  'S', 0x1F, 0x53},
 /* 0x20 - 'd'       */
        { 'd',  'D', 0x20, 0x44},
 /* 0x21 - 'f'       */
        { 'f',  'F', 0x21, 0x46},
 /* 0x22 - 'g'       */
        { 'g',  'G', 0x22, 0x47},
 /* 0x23 - 'h'       */
        { 'h',  'H', 0x23, 0x48},
 /* 0x24 - 'j'       */
        { 'j',  'J', 0x24, 0x4A},
 /* 0x25 - 'k'       */
        { 'k',  'K', 0x25, 0x4B},
 /* 0x26 - 'l'       */
        { 'l',  'L', 0x26, 0x4C},
 /* 0x27 - ';'       */
        { ';',  ':', 0x27, 0xBA},
 /* 0x28 - '\''      */
        {'\'', '\"', 0x28, 0xDE},
 /* 0x29 - '`'       */
        { '`',  '~', 0x29, 0xC0},
 /* 0x2A - l. SHIFT  */
        {   0,    0, 0x2A, 0x10},
 /* 0x2B - '\'       */
        {'\\',  '|', 0x2B, 0xDC},
 /* 0x2C - 'z'       */
        { 'z',  'Z', 0x2C, 0x5A},
 /* 0x2D - 'x'       */
        { 'x',  'X', 0x2D, 0x58},
 /* 0x2E - 'c'       */
        { 'c',  'C', 0x2E, 0x43},
 /* 0x2F - 'v'       */
        { 'v',  'V', 0x2F, 0x56},
 /* 0x30 - 'b'       */
        { 'b',  'B', 0x30, 0x42},
 /* 0x31 - 'n'       */
        { 'n',  'N', 0x31, 0x4E},
 /* 0x32 - 'm'       */
        { 'm',  'M', 0x32, 0x4D},
 /* 0x33 - ','       */
        { ',',  '<', 0x33, 0xBC},
 /* 0x34 - '.'       */
        { '.',  '>', 0x34, 0xBE},
 /* 0x35 - '/'       */
        { '/',  '?', 0x35, 0xBF},
 /* 0x36 - r. SHIFT  */
        {   0,    0, 0x36, 0x10},
 /* 0x37 - '*'       */
        { '*',  '*', 0x37, 0x6A},
 /* 0x38 - ALT       */
        {   0,    0, 0x38, 0x12},
 /* 0x39 - ' '       */
        { ' ',  ' ', 0x39, 0x20},
 /* 0x3A - CapsLock  */
        {   0,    0, 0x3A, 0x14},
 /* 0x3B - F1        */
        {   0,    0, 0x3B, 0x70},
 /* 0x3C - F2        */
        {   0,    0, 0x3C, 0x71},
 /* 0x3D - F3        */
        {   0,    0, 0x3D, 0x72},
 /* 0x3E - F4        */
        {   0,    0, 0x3E, 0x73},
 /* 0x3F - F5        */
        {   0,    0, 0x3F, 0x74},
 /* 0x40 - F6        */
        {   0,    0, 0x40, 0x75},
 /* 0x41 - F7        */
        {   0,    0, 0x41, 0x76},
 /* 0x42 - F8        */
        {   0,    0, 0x42, 0x77},
 /* 0x43 - F9        */
        {   0,    0, 0x43, 0x78},
 /* 0x44 - F10       */
        {   0,    0, 0x44, 0x79},
 /* 0x45 - NumLock   */
        {   0,    0, 0x45, 0x90},
 /* 0x46 - ScrLock   */
        {   0,    0, 0x46, 0x91},
 /* 0x47 - Home      */
        {   0,    0, 0x47, 0x24},
 /* 0x48 - Up        */
        {   0,    0, 0x48, 0x26},
 /* 0x49 - PgUp      */
        {   0,    0, 0x49, 0x21},
 /* 0x4A - '-'       */
        {   0,    0, 0x4A, 0x6D},
 /* 0x4B - Left      */
        {   0,    0, 0x4B, 0x25},
 /* 0x4C - MID       */
        {   0,    0, 0x4C, 0x0C},
 /* 0x4D - Right     */
        {   0,    0, 0x4D, 0x27},
 /* 0x4E - '+'       */
        {   0,    0, 0x4E, 0x6B},
 /* 0x4F - End       */
        {   0,    0, 0x4F, 0x23},
 /* 0x50 - Down      */
        {   0,    0, 0x50, 0x28},
 /* 0x51 - PgDown    */
        {   0,    0, 0x51, 0x22},
 /* 0x52 - Insert    */
        {   0,    0, 0x52, 0x2D},
 /* 0x53 - Del       */
        {   0,    0, 0x53, 0x2E},
 /* 0x54 - Enter     */
        {   0,    0, 0x54, 0x0D},
 /* 0x55 - ???       */
        {   0,    0,    0,    0},
 /* 0x56 - ???       */
        {   0,    0,    0,    0},
 /* 0x57 - F11       */
        {   0,    0, 0x57, 0x7A},
 /* 0x58 - F12       */
        {   0,    0, 0x58, 0x7B},
 /* 0x59 - ???       */
        {   0,    0,    0,    0},
 /* 0x5A - ???       */
        {   0,    0,    0,    0},
 /* 0x5B - Left Win  */
        {   0,    0, 0x5B, 0x5B},
 /* 0x5C - Right Win */
        {   0,    0, 0x5C, 0x5C},
 /* 0x5D - Apps      */
        {   0,    0, 0x5D, 0x5D},
 /* 0x5E - Pause     */
        {   0,    0, 0x5E, 0x13}
};

static void StoreKeyCode(uint32_t keycode)
{
    KeyCodeNode_t* node = malloc(sizeof(KeyCodeNode_t));

    if (!IsEqual(node, nullptr))
    {
        node->keycode = keycode;

        Queue_Add(pKeyCodeQueue, StructOffset(node, KeyCodeNode_t, head));
    }
}

uint32_t FetchKeyCode()
{
    uint32_t ret = 0u;

    if (!Queue_IsEmpty(pKeyCodeQueue))
    {
        KeyCodeNode_t* node = List_Node(Queue_Front(pKeyCodeQueue), KeyCodeNode_t, head);

        ret = node->keycode;

        Queue_Remove(pKeyCodeQueue);

        free(node);
    }

    return ret;
}

static KeyState KeyType(uint8_t scancode)
{
    return (scancode & 0x80) ? KeyRelease : KeyPress;
}

static bool IsShift(uint8_t scancode)
{
    return IsEqual(scancode, 0x2a) || IsEqual(scancode, 0x36);
}

static bool IsCapsLock(uint8_t scancode)
{
    return IsEqual(scancode, 0x3a);
}

static bool IsNumLock(uint8_t scancode)
{
    return IsEqual(scancode, 0x45);
}

static bool IsNumPadKey(uint8_t scancode, bool E0)
{
    static const uint8_t cNumScancode[]               = {0x52, 0x53, 0x4f, 0x50, 0x51, 0x4b, 0x4c, 0x4d, 0x47, 0x48, 0x49, 0x35, 0x37, 0x4a, 0x4e, 0x1c};
    static const bool cNumE0[ArraySize(cNumScancode)] = {false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, true};

    for (uint8_t i = 0; i < ArraySize(cNumScancode); ++i)
    {
        uint8_t* pc = AddrOffset(cNumScancode, i);
        bool* pe    = AddrOffset(cNumE0, i);

        if ((IsEqual(scancode, *pc) && IsEqual(E0, *pe)))
        {
            return true;
        }
    }

    return false;
}

static bool PauseHandler(uint8_t scancode)
{
    static uint8_t cPause = 0u;

    bool ret = IsEqual(scancode, 0xe1) || cPause;

    if (ret)
    {
        static uint8_t cPauseCode[] = {0xe1, 0x1d, 0x45, 0xe1, 0x9d, 0xc5};

        uint8_t* pcc = AddrOffset(cPauseCode, cPause);

        if (IsEqual(scancode, *pcc))
        {
            cPause++;
        }
        else
        {
            cPause = 0u;
            ret    = false;
        }

        if (IsEqual(cPause, ArraySize(cPauseCode)))
        {
            cPause = 0u;
            PutScanCode(0x5e);
            PutScanCode(0xde);
        }
    }

    return ret;
}

static uint32_t MakeNormalCode(KeyCode_t* pkc, bool shift, bool caps)
{
    if (!caps)
    {
        if (!shift)
        {
            return (pkc->scode << 16u) | (pkc->kcode << 8u) | (pkc->ascii1);
        }
        else
        {
            return (pkc->scode << 16u) | (pkc->kcode << 8u) | (pkc->ascii2);
        }
    }
    else
    {
        if (('a' <= pkc->ascii1) && (pkc->ascii1 <= 'z'))
        {
            if (!shift)
            {
                return (pkc->scode << 16u) | (pkc->kcode << 8u) | (pkc->ascii2);
            }
            else
            {
                return (pkc->scode << 16u) | (pkc->kcode << 8u) | (pkc->ascii1);
            }
        }
        else
        {
            if (!shift)
            {
                return (pkc->scode << 16u) | (pkc->kcode << 8u) | (pkc->ascii1);
            }
            else
            {
                return (pkc->scode << 16u) | (pkc->kcode << 8u) | (pkc->ascii2);
            }
        }
    }

    return 0u;
}

static uint32_t MakeNumCode(KeyCode_t* pkc, bool shift, bool num)
{
    static const KeyCode_t cNumKeyMap[] =
        {
            {'0',   0, 0x52, 0x2D},
            {'.',   0, 0x53, 0x2E},
            {'1',   0, 0x4F, 0x23},
            {'2',   0, 0x50, 0x28},
            {'3',   0, 0x51, 0x22},
            {'4',   0, 0x4B, 0x25},
            {'5',   0, 0x4C, 0x0C},
            {'6',   0, 0x4D, 0x27},
            {'7',   0, 0x47, 0x24},
            {'8',   0, 0x48, 0x26},
            {'9',   0, 0x49, 0x21},
            {'/', '/', 0x35, 0x6F},
            {'*', '*', 0x37, 0x6A},
            {'-', '-', 0x4A, 0x6D},
            {'+', '+', 0x4E, 0x6B},
            {  0,   0, 0x1C, 0x0D},
            {  0,   0,    0,    0}
    };

    uint8_t i = 0u;

    KeyCode_t* nkc = AddrOffset(cNumKeyMap, i);

    while (nkc->scode)
    {
        if (IsEqual(nkc->scode, pkc->scode))
        {
            pkc = nkc;
            break;
        }

        nkc = AddrOffset(cNumKeyMap, ++i);
    }

    if (IsEqual(nkc, pkc))
    {
        if (!num)
        {
            return (pkc->scode << 16u) | (pkc->kcode << 8u) | pkc->ascii2;
        }
        else
        {
            if (!shift)
            {
                return (pkc->scode << 16u) | (pkc->kcode << 8u) | pkc->ascii1;
            }
            else
            {
                return (pkc->scode << 16u) | (pkc->kcode << 8u) | pkc->ascii2;
            }
        }
    }

    return 0u;
}

static uint32_t MakeCode(KeyCode_t* pkc, bool shift, bool capsLock, bool numLock, bool E0)
{
    if (IsNumPadKey(pkc->scode, E0))
    {
        return MakeNumCode(pkc, shift, numLock);
    }
    else
    {
        return MakeNormalCode(pkc, shift, capsLock);
    }
}

static bool KeyHandler(uint8_t scancode)
{
    static bool cShift    = false;
    static bool cCapsLock = false;
    static bool cNumLock  = false;
    static bool E0        = false;

    bool ret = false;

    if (ret = IsEqual(scancode, 0xe0))
    {
        E0 = true;
    }
    else
    {
        KeyState pressed = KeyType(scancode);
        KeyCode_t* pkc   = nullptr;

        if (!pressed)
        {
            scancode -= 0x80;
        }

        pkc = AddrOffset(gKeyMap, scancode);

        if (ret = !IsEqual(pkc->scode, 0))
        {
            if (IsShift(scancode))
            {
                cShift = pressed;
            }
            else if (IsCapsLock(scancode) && pressed)
            {
                cCapsLock = !cCapsLock;
            }
            else if (IsNumLock(scancode) && pressed)
            {
                cNumLock = !cNumLock;
            }

            StoreKeyCode((pressed | MakeCode(pkc, cShift, cCapsLock, cNumLock, E0)));

            E0 = false;
        }
    }

    return ret;
}

void PutScanCode(uint8_t scancode)
{
    if (PauseHandler(scancode))
    {
        /* Pause Key */
    }
    else if (KeyHandler(scancode))
    {
        /* Normal Key */
    }
    else
    {
        /* Unknown Key */
    }
}

void KeyBoardModuleInit()
{
    pKeyCodeQueue = malloc(sizeof(Queue_t));

    Queue_Init(pKeyCodeQueue);

    KeyBoardInit();
}
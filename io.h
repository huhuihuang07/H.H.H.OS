#ifndef IO_H
#define IO_H

#include "type.h"

extern u8 inb(u16 port);
extern u16 inw(u16 port);

extern void outb(u16 port, u8 value);
extern void outw(u16 port, u16 value);

#endif //!IO_H
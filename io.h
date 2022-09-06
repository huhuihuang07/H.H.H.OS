#ifndef IO_H
#define IO_H

#include "type.h"

extern const u8 inb(const u16 port);
extern const u16 inw(const u16 port);

extern void outb(const u16 port, const u8 value);
extern void outw(const u16 port, const u16 value);

#endif //!IO_H
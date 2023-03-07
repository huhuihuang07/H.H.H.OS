#pragma once

#include "type.h"

extern const uint8_t inb(const uint16_t port);
extern const uint16_t inw(const uint16_t port);

extern void outb(const uint16_t port, const uint8_t value);
extern void outw(const uint16_t port, const uint16_t value);
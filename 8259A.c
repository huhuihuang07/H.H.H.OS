#include "8259A.h"
#include "io.h"

u8 ReadIMR(u16 port)
{
	return inb(port);
}

void WriteIMR(u16 port, u8 value)
{
	outb(port, value);
}

void SendEOI(u16 port)
{
	outb(port, 0x20);
}

void SetIF()
{
	asm volatile("sti");
}

void ClearIF()
{
	asm volatile("cli");
}

void Init8259A()
{
	// initialize Master 8259A
	outb(MASTER_ICW1_PORT, 0b00010001);
	outb(MASTER_ICW2_PORT, 0x20);
	outb(MASTER_ICW3_PORT, 0b00000100);
	outb(MASTER_ICW4_PORT, 0b00010001);

	// initialize SLAVE 8259A
	outb(SLAVE_ICW1_PORT, 0b00010001);
	outb(SLAVE_ICW2_PORT, 0x28);
	outb(SLAVE_ICW3_PORT, 0b00000010);
	outb(SLAVE_ICW4_PORT, 0b00000001);

	// close all interrupt
	WriteIMR(MASTER_IMR_PORT, 0xff);

	WriteIMR(SLAVE_IMR_PORT, 0xff);

	// open IF
	SetIF();	
}

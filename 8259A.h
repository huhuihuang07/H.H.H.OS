#ifndef I_8259A_H
#define I_8259A_H

#include "type.h"

// PIC-8259A Ports
#define MASTER_ICW1_PORT  0x20
#define MASTER_ICW2_PORT  0x21
#define MASTER_ICW3_PORT  0x21
#define MASTER_ICW4_PORT  0x21
#define MASTER_OCW1_PORT  0x21
#define MASTER_OCW2_PORT  0x20
#define MASTER_OCW3_PORT  0x20

#define SLAVE_ICW1_PORT   0xa0
#define SLAVE_ICW2_PORT   0xa1
#define SLAVE_ICW3_PORT   0xa1
#define SLAVE_ICW4_PORT   0xa1
#define SLAVE_OCW1_PORT   0xa1
#define SLAVE_OCW2_PORT   0xa0
#define SLAVE_OCW3_PORT   0xa0

#define MASTER_EOI_PORT   0x20
#define MASTER_IMR_PORT   0x21
#define MASTER_IRR_PORT   0x20
#define MASTER_ISR_PORT   0x20

#define SLAVE_EOI_PORT    0xa0
#define SLAVE_IMR_PORT    0xa1
#define SLAVE_IRR_PORT    0xa0
#define SLAVE_ISR_PORT    0xa0

void Init8259A();
u8 ReadIMR(u16 port);
void WriteIMR(u16 port, u8 value);
void SendEOI(u16 port);

void SetIF();
void ClearIF();

#endif //!I_8259A_H
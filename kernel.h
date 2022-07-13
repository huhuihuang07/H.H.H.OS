#ifndef KERNEL_H
#define KERNEL_H

#include "type.h"
#include "const.h"

typedef struct{
	u16 limit1;
	u16  base1;
	u8   base2;
	u8   attr1;
	u8   attr2_limit2;
	u8   base3;
}Descriptor;

typedef struct{
	Descriptor* const entry;
	const u16 size;
}GdtInfo;

typedef struct{
	u16 offset1;
	u16 selector;
	u8  paramterCount;
	u8	attribute;
	u16 offset2;
}Gate;

typedef struct{
	Gate* const entry;
	const u16 size;
}IdtInfo;

typedef struct{
	u32 BaseAddrLow;
	u32 BaseAddrHigh;
	u32 LengthLow;
	u32 LengthHigh;
	u32 Type;
}ARDS;

#pragma pack(1)

typedef struct{
	u8 ARDSNumber;
	ARDS ards[20];
}MemInfo;

#pragma pack()

bool setDescValue(Descriptor* pDescriptor, u32 base, u32 limit, u16 attribute);
bool getDescValue(Descriptor* pDescriptor, u32* pBase, u32* pLimit, u16* pAttribute);

bool getGateValue(Gate* pGate, u16 selector, u32 offset, u8 paramterCount, u8 attribute);
bool setGateValue(Gate* pGate, u16* pSelector, u32* pOffset, u8* pParamterCount, u8* pAttribute);

#endif //!KERNEL_H
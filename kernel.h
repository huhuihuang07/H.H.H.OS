#pragma once

#include "type.h"
#include "const.h"

typedef struct
{
	uint16_t limit1;
	uint16_t base1;
	uint8_t base2;
	uint8_t attr1;
	uint8_t attr2_limit2;
	uint8_t base3;
} Descriptor_t;

typedef struct
{
	Descriptor_t *const entry;
	const uint16_t size;
} GdtInfo_t;

typedef struct
{
	uint16_t offset1;
	uint16_t selector;
	uint8_t paramterCount;
	uint8_t attribute;
	uint16_t offset2;
} Gate_t;

typedef struct
{
	Gate_t *const entry;
	const uint16_t size;
} IdtInfo_t;

typedef struct
{
	uint32_t BaseAddrLow;
	uint32_t BaseAddrHigh;
	uint32_t LengthLow;
	uint32_t LengthHigh;
	uint32_t Type;
} ARDS_t;

typedef struct
{
	const uint8_t ARDSNumber;
	const ARDS_t ards[20];
} _packed MemInfo_t;

const GdtInfo_t gGdtInfo;
const IdtInfo_t gIdtInfo;
const MemInfo_t gMemInfo;
const uint32_t gMemSize;

bool SetDescValue(Descriptor_t *pDescriptor, uint32_t base, uint32_t limit, uint16_t attribute);
bool GetDescValue(Descriptor_t *pDescriptor, uint32_t *pBase, uint32_t *pLimit, uint16_t *pAttribute);

bool SetGateValue(Gate_t *pGate, uint16_t selector, uint32_t offset, uint8_t paramterCount, uint8_t attribute);
bool GetGateValue(Gate_t *pGate, uint16_t *pSelector, uint32_t *pOffset, uint8_t *pParamterCount, uint8_t *pAttribute);
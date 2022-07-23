#include "kernel.h"

bool SetDescValue(Descriptor* pDescriptor, u32 base, u32 limit, u16 attribute)
{
	bool ret = (nullptr != pDescriptor);

	if(ret){
		pDescriptor->limit1 = limit & 0xffff;
		pDescriptor->base1  = base & 0xffff;
		pDescriptor->base2  = (base >> 16) & 0xff;
		pDescriptor->attr1  = attribute & 0xff;
		pDescriptor->attr2_limit2 = ((attribute >> 8) & 0xf0) | ((limit >> 16) & 0x0f);
		pDescriptor->base3  = (base >> 24) & 0xff;
	}

	return ret;
}

bool GetDescValue(Descriptor* pDescriptor, u32* pBase, u32* pLimit, u16* pAttribute)
{
	bool ret = pDescriptor && pBase && pLimit && pAttribute;

	if(ret){
		*pBase = (pDescriptor->base3 << 24) | (pDescriptor->base2 << 16) | (pDescriptor->base1);
		*pLimit = ((pDescriptor->attr2_limit2 & 0x0f) << 16) | (pDescriptor->limit1);
		*pAttribute = ((pDescriptor->attr2_limit2 & 0xf0) << 8) | (pDescriptor->attr1);
	}

	return ret;
}

bool SetGateValue(Gate* pGate, u16 selector, u32 offset, u8 paramterCount, u8 attribute)
{
	bool ret = (nullptr != pGate);

	if(ret){
		pGate->offset1 = offset & 0xffff;
		pGate->selector = selector;
		pGate->paramterCount = paramterCount & 0x1f;
		pGate->attribute = attribute;
		pGate->offset2 = (offset >> 16) & 0xffff;
	}

	return ret;
}

bool GetGateValue(Gate* pGate, u16* pSelector, u32* pOffset, u8* pParamterCount, u8* pAttribute)
{
	bool ret = pGate && pSelector && pOffset && pParamterCount && pAttribute;

	if(ret){
		*pSelector = pGate->selector;
		*pOffset = (pGate->offset2 << 16) | (pGate->offset1);
		*pParamterCount = pGate->paramterCount;
		*pAttribute = pGate->attribute;
	}

	return ret;
}
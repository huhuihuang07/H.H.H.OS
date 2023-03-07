#include "kernel.h"
#include "utility.h"

const GdtInfo_t gGdtInfo = {nullptr, 0};
const IdtInfo_t gIdtInfo = {nullptr, 0};
const MemInfo_t gMemInfo = {0};

const uint32_t gMemSize = 0;

bool SetDescValue(Descriptor_t* pDescriptor, uint32_t base, uint32_t limit, uint16_t attribute)
{
    bool ret = !IsEqual(pDescriptor, nullptr);

    if (ret)
    {
        pDescriptor->limit1 = limit & 0xffff;
        pDescriptor->base1 = base & 0xffff;
        pDescriptor->base2 = (base >> 16) & 0xff;
        pDescriptor->attr1 = attribute & 0xff;
        pDescriptor->attr2_limit2 = ((attribute >> 8) & 0xf0) | ((limit >> 16) & 0x0f);
        pDescriptor->base3 = (base >> 24) & 0xff;
    }

    return ret;
}

bool GetDescValue(Descriptor_t* pDescriptor, uint32_t* pBase, uint32_t* pLimit, uint16_t* pAttribute)
{
    bool ret = pDescriptor && pBase && pLimit && pAttribute;

    if (ret)
    {
        *pBase = (pDescriptor->base3 << 24) | (pDescriptor->base2 << 16) | (pDescriptor->base1);
        *pLimit = ((pDescriptor->attr2_limit2 & 0x0f) << 16) | (pDescriptor->limit1);
        *pAttribute = ((pDescriptor->attr2_limit2 & 0xf0) << 8) | (pDescriptor->attr1);
    }

    return ret;
}

bool SetGateValue(Gate_t* pGate, uint16_t selector, uint32_t offset, uint8_t paramterCount, uint8_t attribute)
{
    bool ret = !IsEqual(pGate, nullptr);

    if (ret)
    {
        pGate->offset1 = offset & 0xffff;
        pGate->selector = selector;
        pGate->paramterCount = paramterCount & 0x1f;
        pGate->attribute = attribute;
        pGate->offset2 = (offset >> 16) & 0xffff;
    }

    return ret;
}

bool GetGateValue(Gate_t* pGate, uint16_t* pSelector, uint32_t* pOffset, uint8_t* pParamterCount, uint8_t* pAttribute)
{
    bool ret = pGate && pSelector && pOffset && pParamterCount && pAttribute;

    if (ret)
    {
        *pSelector = pGate->selector;
        *pOffset = (pGate->offset2 << 16) | (pGate->offset1);
        *pParamterCount = pGate->paramterCount;
        *pAttribute = pGate->attribute;
    }

    return ret;
}
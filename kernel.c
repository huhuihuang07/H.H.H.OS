#include "kernel.h"
#include "utility.h"

const GdtInfo_t gGdtInfo = {nullptr, 0u};
const IdtInfo_t gIdtInfo = {nullptr, 0u};
const MemInfo_t gMemInfo = {0u};

const uint32_t gMemSize = 0u;

bool SetDescValue(Descriptor_t* pDescriptor, uint32_t base, uint32_t limit, uint16_t attribute)
{
    bool ret = !IsEqual(pDescriptor, nullptr);

    if (ret)
    {
        pDescriptor->limit1       = limit & 0xffff;
        pDescriptor->base1        = base & 0xffff;
        pDescriptor->base2        = (base >> 16u) & 0xff;
        pDescriptor->attr1        = attribute & 0xff;
        pDescriptor->attr2_limit2 = ((attribute >> 8u) & 0xf0) | ((limit >> 16u) & 0x0f);
        pDescriptor->base3        = (base >> 24u) & 0xff;
    }

    return ret;
}

bool GetDescValue(Descriptor_t* pDescriptor, uint32_t* pBase, uint32_t* pLimit, uint16_t* pAttribute)
{
    bool ret = pDescriptor && pBase && pLimit && pAttribute;

    if (ret)
    {
        *pBase      = (pDescriptor->base3 << 24u) | (pDescriptor->base2 << 16u) | (pDescriptor->base1);
        *pLimit     = ((pDescriptor->attr2_limit2 & 0x0f) << 16u) | (pDescriptor->limit1);
        *pAttribute = ((pDescriptor->attr2_limit2 & 0xf0) << 8u) | (pDescriptor->attr1);
    }

    return ret;
}

bool SetGateValue(Gate_t* pGate, uint16_t selector, uint32_t offset, uint8_t parameterCount, uint8_t attribute)
{
    bool ret = !IsEqual(pGate, nullptr);

    if (ret)
    {
        pGate->offset1        = offset & 0xffff;
        pGate->selector       = selector;
        pGate->parameterCount = parameterCount & 0x1f;
        pGate->attribute      = attribute;
        pGate->offset2        = (offset >> 16u) & 0xffff;
    }

    return ret;
}

bool GetGateValue(Gate_t* pGate, uint16_t* pSelector, uint32_t* pOffset, uint8_t* pParameterCount, uint8_t* pAttribute)
{
    bool ret = pGate && pSelector && pOffset && pParameterCount && pAttribute;

    if (ret)
    {
        *pSelector       = pGate->selector;
        *pOffset         = (pGate->offset2 << 16u) | (pGate->offset1);
        *pParameterCount = pGate->parameterCount;
        *pAttribute      = pGate->attribute;
    }

    return ret;
}
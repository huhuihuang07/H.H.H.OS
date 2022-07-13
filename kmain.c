#include "kmain.h"
#include "kernel.h"
#include "screen.h"
#include "global.h"

int KMain(int argc, char* argv[])
{
	printf("argc : %d\n", argc);
	
	for(int i = 0; i != argc; ++i){
		printf("argv : %s\n", argv[i]);
	}

	printf("Memory Size : %p\n", gMemSize);

	for(u8 i = 0; i != gMemInfo.ARDSNumber; ++i){
		printf("BaseAddrLow : %p, BaseAddrHigh : %p, LengthLow : %p, LengthHigh : %p, Type : %p\n", gMemInfo.ards[i].BaseAddrLow, gMemInfo.ards[i].BaseAddrHigh, gMemInfo.ards[i].LengthLow, gMemInfo.ards[i].LengthHigh, gMemInfo.ards[i].Type);
	}

	printf("GDT entry : %p, GDT size : %d\n", gGdtInfo.entry, gGdtInfo.size);
	printf("IDT entry : %p, IDT size : %d\n", gIdtInfo.entry, gIdtInfo.size);

	for(u16 i = 0; i != gGdtInfo.size; ++i){
		u32 base = 0;
		u32 limit = 0;
		u16 attribute = 0;

		getDescValue(gGdtInfo.entry + i, &base, &limit, &attribute);

		printf("base : %p, limit : %p, attribute : %p\n", base, limit, attribute);
	}

	return 0;
}
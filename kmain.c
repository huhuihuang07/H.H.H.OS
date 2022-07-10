#include "kmain.h"
#include "screen.h"

int KMain(int argc, char* argv[])
{
	printf("argc : %d\n", argc);
	
	for(int i = 0; i != argc; ++i){
		printf("argv : %s\n", argv[i]);
	}

	return 0;
}
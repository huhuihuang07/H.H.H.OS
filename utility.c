#include <utility.h>

void Delay(u8 n)
{
	while(n--){
		for(int i = 0; !IsEqual(i, 1000); ++i){
			for(int j = 0; !IsEqual(j, 1000); ++j){
				asm volatile("nop");
			}
		}
	}
}

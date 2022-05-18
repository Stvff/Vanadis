// this is a theoretical version of trans-/compiled vanadis to give me an idea for what the 'compiler' needs to do
#include "../vic.h"
#include <time.h>

void speedmain(){
	uint64_t dummy = 0;
	time_t thetime;
	inttonry(&regs[formr], 1, SN regs[formr].base);
	inttonry(&regs[gr1], 30000000, (UN regs[formr].base)%4);
	thetime = time(&thetime); inttonry(&regs[tme], (uint64_t)thetime, (UN regs[formr].base)%4);
	pushtost(&regs[tme]);
	speedmainrmr1:
	SL regs[ir].fst += 1;
	if(SL regs[ir].fst == SL regs[gr1].fst) SN regs[flag].base = 0;
	else if(SL regs[ir].fst >  SL regs[gr1].fst) SN regs[flag].base = 1;
	else SN regs[flag].base = 2;
	if(SN regs[flag].base == 2){ goto speedmainrmr1;}
	printf("%ld%c", SL regs[ir].fst, 0 == 0 ? '\n' : '\0');
	SL stack[0 + UL regs[offset].fst]->fst = SL regs[tme].fst - SL stack[0 + UL regs[offset].fst]->fst;
	printf("%ld%c", SL stack[0 + UL regs[offset].fst]->fst, 0 == 0 ? '\n' : '\0');	
}

int main(){
	for(int i = 0; i < regAmount; i++){
		makenry(&regs[i], 8);
		memset(regs[i].base, 0, 8);
	}
	initmac();
	speedmain();	
	freemac();
	return 0;
}

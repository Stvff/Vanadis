#ifndef VCOMPILER_H
#define VCOMPILER_H
#include "vic.h"
#include <time.h>

nry_t* far(nry_t* str, nry_t* find, nry_t* repl){
	
	return str;
}

nry_t* regstrs(int ins, nry_t* str){
	nry_t dunry;
	makenry(&dunry, 0);
	switch (ins){
		case gr1: strcpytonry(str, "&regs[gr1]."); break;
		case gr2: strcpytonry(str, "&regs[gr2]."); break;
		case gr3: strcpytonry(str, "&regs[gr3]."); break;
		case gr4: strcpytonry(str, "&regs[gr4]."); break;
		case gr5: strcpytonry(str, "&regs[gr5]."); break;
		case gr6: strcpytonry(str, "&regs[gr6]."); break;
		case ir: strcpytonry(str, "&regs[ir]."); break;
		case jr: strcpytonry(str, "&regs[jr]."); break;
		case ans: strcpytonry(str, "&regs[ans]."); break;
		case offset: strcpytonry(str, "&regs[offset]."); break;
		case flag: strcpytonry(str, "&regs[offset]."); break;
		case stkptr: strcpytonry(str, "inttonry(&regs[stkptr], stackPtr, 1)->"); break;
		case cdxptr: strcpytonry(str, "inttonry(&regs[cdxptr], codexPtr, 1)->"); break;
//		case : strcpytonry(str, ""); break;
	}
	freenry(&dunry);
	return str;
}

// %a1 -- %a3 are the arguments
// a : directly after an argument is -> || .
//a p directly after an argument is & before it || nothing
// %# is SN || SL || UN || UL
// %U is UN || UL
// %S is SN || SL

nry_t* instrstrs(int ins, nry_t* str){
	nry_t dunry;
	makenry(&dunry, 0);
	switch (ins){
		case set: strcpytonry(str, "copynry(%a0p, %a1p);\n"); break;
		case dset: strcpytonry(str, "%# %a0:fst = %# %a1:fst;\n"); break;
		case iget: strcpytonry(str, "%# %a0:fst = %# (%a1:base + (%U %a2:fst) % %a1:len);\n"); break;
		case iset: strcpytonry(str, "%# (%a0:base + (%U %a2:fst) % %a0:len) = %# %a1:fst;\n"); break;
		case inc: strcpytonry(str, "%# %a0:fst += 1;\n"); break;
		case dec: strcpytonry(str, "%# %a0:fst -= 1;\n"); break;
		case add: strcpytonry(str, "%# %a0:fst += %# %a1:fst;\n"); break;
		case sub: strcpytonry(str, "%# %a0:fst -= %# %a1:fst;\n"); break;
		case rsub: strcpytonry(str, "%# %a0:fst = %# %a1:fst - %# %a0:fst;\n"); break;
		case mul: strcpytonry(str, "%# %a0:fst *= %# %a1:fst;\n"); break;
		case divi: strcpytonry(str, "%# %a0:fst /= %# %a1:fst;\n"); break;
		case modu: strcpytonry(str, "%# %a0:fst %= %# %a1:fst;\n"); break;
		case app: strcpytonry(str, "appendnry(%a0p, %a1p);\n"); break;
		case shf: strcpytonry(str, "shiftnry(%a0p, %# %a1:fst, form);\n"); break;
		case rshf: strcpytonry(str, "rshiftnry(%a0p, %# %a1:fst, form);\n"); break;
		case sel: strcpytonry(str, "cutnry(%a0p, %a1p, %# %a2:fst, (%# %a2:fst) + (%# %a0:fst));\n"); break;
		case cut: strcpytonry(str, "cutnry(%a0p, %a0p, %# %a1:fst, %# %a2:fst);\n"); break;
		case ptra: strcpytonry(str, "%a0:fst = %a0:base + ((%a0:fst-%a0:base) + %# %a1:fst) % %a0:len;\n"); break;
		case ptrs: strcpytonry(str, "%a0:fst = %a0:base + ((%a0:fst-%a0:base) - %# %a1:fst) % %a0:len;\n"); break;
		case ptr: strcpytonry(str, "%a0:fst = %a0:base + (%U %a1:fst) % %a0:len;\n"); break;
		case len: strcpytonry(str, "inttonry(%a0p, %a1:len, 3);\n"); break;
		case fstF: strcpytonry(str, "inttonry(%a0p, %a1:fst - %a1:base, 3);\n"); break;
		case push: strcpytonry(str, "pushtost(%a0p);\n"); break;
		case pop: strcpytonry(str, "popfromst(%a0p);\n"); break;
		case peek: 
			strcpytonry(str, "{if(stackPtr == -1) printf(\"\\aThere are no elements on the stack\\n\");\n");
			appendnry(str, strcpytonry(&dunry, "else copynry(%a0p, stack[stackPtr]);}\n"));
			break;
		case flip:
			break;
			strcpytonry(str, "{dummy = (uint64_t) %# %a0:fst;\n");
			appendnry(str, strcpytonry(&dunry, "if(dummy == 0) dummy = 1;\n"));
			appendnry(str, strcpytonry(&dunry, "for(uint64_t i = 0; i < dummy; i++) if(!Unflip()) break;}\n"));
		case unf:
			strcpytonry(str, "{dummy = (uint64_t) %# %a0:fst;\n");
			appendnry(str, strcpytonry(&dunry, "if(dummy == 0) dummy = 1;\n"));
			appendnry(str, strcpytonry(&dunry, "for(uint64_t i = 0; i < dummy; i++) if(!Unflip()){ dummy = -117; break;};\n"));
			appendnry(str, strcpytonry(&dunry, "if(dummy != (uint64_t) -117 && SN %a1:fst != 0) copynry(&regs[ans], stack[stackPtr]);}\n"));
			break;
		case cmp:
			strcpytonry(str, "{if(%# %a0:fst == %# %a1:fst) SN regs[flag].base = 0;\n");
			appendnry(str, strcpytonry(&dunry, "else if(%# %a0:fst < %# %a1:fst) SN regs[flag].base = 2;\n"));
			appendnry(str, strcpytonry(&dunry, "else SN regs[flag].base = 2;}\n"));
			break;
		case equ:
			strcpytonry(str, "{if(equalnry(%a0p, %a1p)) SN regs[flag].base = 0;\n");
			appendnry(str, strcpytonry(&dunry, "else SN regs[flag].base = 3;}\n"));
			break;
		case ninput: strcpytonry(str, "//TODO ninput;\n"); break;
		case sinput: strcpytonry(str, "//TODO sinput;\n"); break;
		case print: strcpytonry(str, "aprintnry(%a0p, form, SN %a0:fst == 0);\n"); break;
		case nprint: strcpytonry(str, "printf(\"%d%c\", %# %a0:fst, SN %a1:fst == 0 ? '\\n' : '\\0';\n"); break;
		case sprint:
			strcpytonry(str, "{%a0:base[%a0:len] = '\\0';\n");
			appendnry(str, strcpytonry(&dunry, "printf(\"%s%c\", %a0:fst, SN %a1:fst ? '\\n' : '\\0');}\n"));
			break;
		case firead:
			strcpytonry(str, "//TODO firead;\n");
			break;
		case fiwrite:
			strcpytonry(str, "//TODO fiwrite;\n");
			break;
		case flen:
			strcpytonry(str, "//TODO flen;\n");
			break;
		case jmp: strcpytonry(str, "goto %a;\n"); break;

//		case : strcpytonry(str, ";\n"); break;
//			appendnry(str, strcpytonry(&dunry, ";\n"));
	}
	freenry(&dunry);
	return str;
}
#endif

#ifndef VINTERPRETER_H
#define VINTERPRETER_H
#include "vic.h"
#include <time.h>

// ######################################################################################## execs
nry_t* exec(int ins, nry_t** args, bool* doprint){
	nry_t* retptr = args[0];
	switch (ins) {
// set
		case set*4 ... set*4 + 3: copynry(args[0], args[1]); break;

// dset
		case dset*4:     SN args[0]->fst = SN args[1]->fst; break;
		case dset*4 + 1: SL args[0]->fst = SL args[1]->fst; break; // l
		case dset*4 + 2: UN args[0]->fst = UN args[1]->fst; break; // u
		case dset*4 + 3: UL args[0]->fst = UL args[1]->fst; break; // u l

// iget
		case iget*4:     SN args[0]->fst = SN (args[1]->base + (UN args[2]->fst)%args[1]->len); break;
		case iget*4 + 1: SL args[0]->fst = SL (args[1]->base + (UL args[2]->fst)%args[1]->len); break; // l
		case iget*4 + 2: UN args[0]->fst = UN (args[1]->base + (UN args[2]->fst)%args[1]->len); break; // u
		case iget*4 + 3: UL args[0]->fst = UL (args[1]->base + (UL args[2]->fst)%args[1]->len); break; // u l

// iset
		case iset*4:     SN (args[0]->base + (UN args[2]->fst)%args[0]->len) = SN args[1]->fst; break;
		case iset*4 + 1: SL (args[0]->base + (UL args[2]->fst)%args[0]->len) = SL args[1]->fst; break;
		case iset*4 + 2: UN (args[0]->base + (UN args[2]->fst)%args[0]->len) = UN args[1]->fst; break;
		case iset*4 + 3: UL (args[0]->base + (UL args[2]->fst)%args[0]->len) = UL args[1]->fst; break;

// inc
		case inc*4:     SN args[0]->fst += 1; break;
		case inc*4 + 1: SL args[0]->fst += 1; break; // l
		case inc*4 + 2: UN args[0]->fst += 1; break; // u
		case inc*4 + 3: UL args[0]->fst += 1; break; // u l

// dec
		case dec*4:     SN args[0]->fst -= 1; break;
		case dec*4 + 1: SL args[0]->fst -= 1; break; // l
		case dec*4 + 2: UN args[0]->fst -= 1; break; // u
		case dec*4 + 3: UL args[0]->fst -= 1; break; // u l

// add
		case add*4:     SN args[0]->fst += SN args[1]->fst; break;
		case add*4 + 1: SL args[0]->fst += SL args[1]->fst; break; // l
		case add*4 + 2: UN args[0]->fst += UN args[1]->fst; break; // u
		case add*4 + 3: UL args[0]->fst += UL args[1]->fst; break; // u l

// sub
		case sub*4:     SN args[0]->fst -= SN args[1]->fst; break;
		case sub*4 + 1: SL args[0]->fst -= SL args[1]->fst; break; // l
		case sub*4 + 2: UN args[0]->fst -= UN args[1]->fst; break; // u
		case sub*4 + 3: UL args[0]->fst -= UL args[1]->fst; break; // u l

// rsub
		case rsub*4:     SN args[0]->fst = SN args[1]->fst - SN args[0]->fst; break;
		case rsub*4 + 1: SL args[0]->fst = SL args[1]->fst - SL args[0]->fst; break; // l
		case rsub*4 + 2: UN args[0]->fst = UN args[1]->fst - UN args[0]->fst; break; // u
		case rsub*4 + 3: UL args[0]->fst = UL args[1]->fst - SL args[0]->fst; break; // u l

// mul
		case mul*4:     SN args[0]->fst *= SN args[1]->fst; break;
		case mul*4 + 1: SL args[0]->fst *= SL args[1]->fst; break; // l
		case mul*4 + 2: UN args[0]->fst *= UN args[1]->fst; break; // u
		case mul*4 + 3: UL args[0]->fst *= UL args[1]->fst; break; // u l

// div
		case divi*4:     SN args[0]->fst /= SN args[1]->fst; break;
		case divi*4 + 1: SL args[0]->fst /= SL args[1]->fst; break; // l
		case divi*4 + 2: UN args[0]->fst /= UN args[1]->fst; break; // u
		case divi*4 + 3: UL args[0]->fst /= UL args[1]->fst; break; // u l

// mod
		case modu*4:     SN args[0]->fst %= SN args[1]->fst; break;
		case modu*4 + 1: SL args[0]->fst %= SL args[1]->fst; break; // l
		case modu*4 + 2: UN args[0]->fst %= UN args[1]->fst; break; // u
		case modu*4 + 3: UL args[0]->fst %= UL args[1]->fst; break; // u l

// app
		case app*4 ... app*4 + 3: appendnry(args[0], args[1]); break;
// rot
// shf
		case shf*4:     shiftnry(args[0], SN args[1]->fst, ins%4); break;
		case shf*4 + 1: shiftnry(args[0], SL args[1]->fst, ins%4); break;
		case shf*4 + 2: shiftnry(args[0], UN args[1]->fst, ins%4); break;
		case shf*4 + 3: shiftnry(args[0], UL args[1]->fst, ins%4); break;
// rshf
		case rshf*4:     rshiftnry(args[0], SN args[1]->fst, ins%4); break;
		case rshf*4 + 1: rshiftnry(args[0], SL args[1]->fst, ins%4); break;
		case rshf*4 + 2: rshiftnry(args[0], UN args[1]->fst, ins%4); break;
		case rshf*4 + 3: rshiftnry(args[0], UL args[1]->fst, ins%4); break;

// rev
// sel
		case sel*4:     cutnry(args[0], args[1], SN args[2]->fst, (SN args[2]->fst) + (SN args[0]->fst)); break;
		case sel*4 + 1: cutnry(args[0], args[1], SL args[2]->fst, (SL args[2]->fst) + (SL args[0]->fst)); break;
		case sel*4 + 2: cutnry(args[0], args[1], UN args[2]->fst, (UN args[2]->fst) + (UN args[0]->fst)); break;
		case sel*4 + 3: cutnry(args[0], args[1], UL args[2]->fst, (UL args[2]->fst) + (UL args[0]->fst)); break;

// cut
		case cut*4:     cutnry(args[0], args[0], SN args[1]->fst, SN args[2]->fst); break;
		case cut*4 + 1: cutnry(args[0], args[0], SL args[1]->fst, SL args[2]->fst); break;
		case cut*4 + 2: cutnry(args[0], args[0], UN args[1]->fst, UN args[2]->fst); break;
		case cut*4 + 3: cutnry(args[0], args[0], UL args[1]->fst, UL args[2]->fst); break;

// ins
		case inse*4 ... inse*4 + 3: insertnry(args[0], args[1], nrytoint(args[2], ins%4)); break;

// ptra
		case ptra*4:     args[0]->fst = args[0]->base + ((args[0]->fst-args[0]->base) + SN args[1]->fst) % args[0]->len; break;
		case ptra*4 + 1: args[0]->fst = args[0]->base + ((args[0]->fst-args[0]->base) + SL args[1]->fst) % args[0]->len; break; // l
		case ptra*4 + 2: args[0]->fst = args[0]->base + ((args[0]->fst-args[0]->base) + UN args[1]->fst) % args[0]->len; break; // u
		case ptra*4 + 3: args[0]->fst = args[0]->base + ((args[0]->fst-args[0]->base) + UL args[1]->fst) % args[0]->len; break; // u l

// ptrs
		case ptrs*4:     args[0]->fst = args[0]->base + ((args[0]->fst-args[0]->base) - SN args[1]->fst) % args[0]->len; break;
		case ptrs*4 + 1: args[0]->fst = args[0]->base + ((args[0]->fst-args[0]->base) - SL args[1]->fst) % args[0]->len; break; // l
		case ptrs*4 + 2: args[0]->fst = args[0]->base + ((args[0]->fst-args[0]->base) - UN args[1]->fst) % args[0]->len; break; // u
		case ptrs*4 + 3: args[0]->fst = args[0]->base + ((args[0]->fst-args[0]->base) - UL args[1]->fst) % args[0]->len; break; // u l

// ptr
		case ptr*4:
		case ptr*4 + 2: args[0]->fst = args[0]->base + (UN args[1]->fst)%args[0]->len; break;
		case ptr*4 + 1:
		case ptr*4 + 3: args[0]->fst = args[0]->base + (UL args[1]->fst)%args[0]->len; break;

// len
		case len*4 ... len*4 + 3: inttonry(args[0], args[1]->len, 3); break;
// fst
		case fstF*4 ... fstF*4 + 3: inttonry(args[0], args[1]->fst - args[1]->base, 3); break;

// push
		case push*4 ... push*4 + 3: *doprint &= pushtost(args[0]); break;
// pop
		case pop*4 ... pop*4 + 3: *doprint &= popfromst(args[0]); break;
// peek
		case peek*4 ... peek*4 + 3: if(stackPtr == -1){ printf("\aThere are no elements on the stack\n"); *doprint = false; break;}
			copynry(args[0], stack[stackPtr]); break;
// flip
		case flip*4:     dummy = (uint64_t) SN args[0]->fst; goto trueflip;
		case flip*4 + 1: dummy = (uint64_t) SL args[0]->fst; goto trueflip;
		case flip*4 + 2: dummy = (uint64_t) UN args[0]->fst; goto trueflip;
		case flip*4 + 3: dummy = (uint64_t) UL args[0]->fst;
			trueflip:
			if(dummy == 0) dummy = 1;
			for(uint64_t i = 0; i < dummy; i++) if(!Flip()){ *doprint &= false; dummy = -117; break;};
			if(dummy == (uint64_t) -117) break;
			retptr = codex[codexPtr];
			break;
			
// unf
		case unf*4:     dummy = (uint64_t) SN args[0]->fst; goto trueunf;
		case unf*4 + 1: dummy = (uint64_t) SL args[0]->fst; goto trueunf;
		case unf*4 + 2: dummy = (uint64_t) UN args[0]->fst; goto trueunf;
		case unf*4 + 3: dummy = (uint64_t) UL args[0]->fst;
			trueunf:
			if(dummy == 0) dummy = 1;
			for(uint64_t i = 0; i < dummy; i++) if(!Unflip()){ *doprint &= false; dummy = -117; break;};
			if(dummy == (uint64_t) -117) break;
			retptr = stack[stackPtr];
			if(SN args[1]->fst != 0) copynry(&regs[ans], retptr);
			break;

// cmp
		case cmp*4:
			if(SN args[0]->fst == SN args[1]->fst) SN regs[flag].base = 0;
			else if(SN args[0]->fst >  SN args[1]->fst) SN regs[flag].base = 1;
			else SN regs[flag].base = 2; ;goto truecmp;
		case cmp*4 + 1:
			if(SL args[0]->fst == SL args[1]->fst) SN regs[flag].base = 0;
			else if(SL args[0]->fst >  SL args[1]->fst) SN regs[flag].base = 1;
			else SN regs[flag].base = 2; ;goto truecmp;
		case cmp*4 + 2:
			if(UN args[0]->fst == UN args[1]->fst) SN regs[flag].base = 0;
			else if(UN args[0]->fst >  UN args[1]->fst) SN regs[flag].base = 1;
			else SN regs[flag].base = 2; ;goto truecmp;
		case cmp*4 + 3:
			if(UL args[0]->fst == UL args[1]->fst) SN regs[flag].base = 0;
			else if(UL args[0]->fst >  UL args[1]->fst) SN regs[flag].base = 1;
			else SN regs[flag].base = 2;
			truecmp: retptr = &regs[flag]; break;
// equ
		case equ*4 ... equ*4 + 3:
			if(equalnry(args[0], args[1])) SN regs[flag].base = 0;
			else SN regs[flag].base = 3; ;retptr = &regs[flag]; break;

// ninput
		case ninput*4 ... ninput*4 + 3: if(UN args[2]->fst == 0){
			fgets(UserInput, userInputLen, stdin);
			inttonry(args[0], inputtoint(UserInput, 0, (int*)&dummy, ins%4 < 2), ins%4);} else
			inttonry(args[0], inputtoint((char*) args[1]->fst, 0, (int*)&dummy, ins%4 < 2), ins%4);
			break;

// sinput
		case sinput*4 ... sinput*4 + 3:dummy = 0; if(UN args[2]->fst == 0){
			fgets(UserInput, userInputLen, stdin);
			strcpytonry(args[0], UserInput);} else
			strtonry(args[0], (char*) args[1]->fst, (int*)&dummy);
			break;

// print
		case print*4 ... print*4 + 3:
			aprintnry(args[0], ins%4, SN args[1]->fst == 0);
			break;

// nprint
		case nprint*4:     printf("%d%c",  SN args[0]->fst, SN args[1]->fst == 0 ? '\n' : '\0'); break;
		case nprint*4 + 1: printf("%ld%c", SL args[0]->fst, SN args[1]->fst == 0 ? '\n' : '\0'); break; // l
		case nprint*4 + 2: printf("%d%c",  UN args[0]->fst, SN args[1]->fst == 0 ? '\n' : '\0'); break; // u
		case nprint*4 + 3: printf("%lu%c", UL args[0]->fst, SN args[1]->fst == 0 ? '\n' : '\0'); break; // u l

// sprint
		case sprint*4 ... sprint*4 + 3:
			args[0]->base[args[0]->len] = '\0';
			printf("%s%c", args[0]->fst, SN args[1]->fst == 0 ? '\n' : '\0');
			break;

// firead
		case firead*4: case firead*4 + 2:
			quicfptr = fopen((char*)(args[1]->fst), "rb");
			if(quicfptr == NULL){ printf("\aCould not open file '%s'.\n", args[0]->fst); *doprint &= false; break;}
			dummy = UN args[0]->fst; freenry(args[0]); makenry(args[0], dummy);
			fseek(quicfptr, UN args[2]->fst, SEEK_SET); fread(args[0]->base, 1, dummy, quicfptr);
			fclose(quicfptr); break;
		case firead*4 + 1: case firead*4 + 3:
			quicfptr = fopen((char*)(args[1]->fst), "rb");
			if(quicfptr == NULL){ printf("\aCould not open file '%s'.\n", args[0]->fst); *doprint &= false; break;}
			dummy = UL args[0]->fst; freenry(args[0]); makenry(args[0], dummy);
			fseek(quicfptr, UL args[2]->fst, SEEK_SET); fread(args[0]->base, 1, dummy, quicfptr);
			fclose(quicfptr); break;

// fwrite
		case fiwrite*4: case fiwrite*4 + 2:
			if(UN args[3]->fst == 0) quicfptr = fopen((char*)(args[1]->fst), "wb"); else quicfptr = fopen((char*)(args[1]->fst), "wb+");
			if(quicfptr == NULL){ printf("\aCould not open or create file '%s'.\n", args[1]->fst); doprint = false; break;}
			fseek(quicfptr, UN args[2], SEEK_SET); fwrite(args[0]->base, 1, args[0]->len, quicfptr);
			fclose(quicfptr); break;
		case fiwrite*4 + 1: case fiwrite*4 + 3:
			if(UN args[3]->fst == 0) quicfptr = fopen((char*)(args[1]->fst), "wb"); else quicfptr = fopen((char*)(args[1]->fst), "wb+");
			if(quicfptr == NULL){ printf("\aCould not open or create file '%s'.\n", args[1]->fst); doprint = false; break;}
			fseek(quicfptr, UL args[2], SEEK_SET); fwrite(args[0]->base, 1, args[0]->len, quicfptr);
			fclose(quicfptr); break;

// flen
		case flen*4 ... flen*4 + 3:
			quicfptr = fopen((char*)(args[1]->fst), "r");
			if(quicfptr == NULL){ printf("\aCould not read the length of file '%s'.\n", args[1]->fst); doprint = false; break;}
			fseek(quicfptr, 0, SEEK_END); inttonry(args[0], ftell(quicfptr), ins%4); fclose(quicfptr); break;
	}
	return retptr;
}

void filerelexec(int ins, nry_t** args, file_t** filepp){
	int dummy;
	file_t* file = *filepp;
	switch(ins){
		case rmr*4 ... rmr*4 + 3: inttonry(args[0], file->pos, ins%4); break;
		case rjmp*4: case rjmp*4 + 2: file->pos = UN args[0]->fst; goto truerjmp;
		case rjmp*4 + 1: case rjmp*4 + 3: file->pos = UL args[0]->fst; goto truerjmp;
			truerjmp:
			inttonry(args[1], file->pos, ins%4);
			break;
		case jmp*4 ... jmp*4 + 3:
			dummy = 0;
			dummy = strlook((char*)args[0]->fst, labelSize, file->labels, &dummy);
			if(dummy == -1){ printf("\aLabel '%s' does not exist.\n", args[1]->fst); break;}
			inttonry(args[1], file->pos, ins%4);
			file->pos = file->labelposs[dummy];
			break;
		case run*4 ... run*4 + 3:
			*filepp = openscript((char*)args[0]->fst, file);
			break;
	}
}

#endif

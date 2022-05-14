#ifndef VANADIS_H
#define VANADIS_H
#include "nry.h"

#define userInputLen 256
#define maxKeywordLen 16
#define argumentAmount 4

int strlook(char string[], int thewordlen, char source[][thewordlen], int* readhead){
	int item = 0;
	int j;
	bool isthis;
	while (source[item][0] != '\0'){
		isthis = true;
		j = 0;
		while (source[item][j] != '\0'){
			if(string[j + *readhead] != source[item][j]) isthis = false;
			j++;
		}
		if(isthis){
			*readhead += --j;
			return item;
		}
		item++;
	}
	return -1;
}

char registerString[][maxKeywordLen] = {
	"gr1", "gr2", "gr3", "gr4", "gr5", "gr6",
	"ir", "jr", "ans",
	"offset", "form", "flag",
	"stkptr", "cdxptr",
	"time",
	"\0end"
};

enum registerEnum {
	gr1, gr2, gr3, gr4, gr5, gr6,
	ir, jr, ans,
	offset, formr, flag,
	stkptr, cdxptr, tme,
	regAmount
};

char instructionString[][maxKeywordLen] = {
	"set", "dset", "iget", "iset",
	"inc", "dec", "add", "sub", "mul", "div", "mod",
	"app", "rot", "shf", "rev", "sel", "cut", "ins",
	"ptra", "ptrs", "ptr", "len", "fst",
	"push", "pop", "flip", "unf",
	"cmp", "equ", "Ce", "Cs", "Cg", "Cn",
	"fread", "fwrite", "flen",
	"rmr", "jmp", "rjmp",
	"print", "nprint", "sprint",
	"\\", "\0end"
};

enum instructionEnum {
	set, dset, iget, iset,
	inc, dec, add, sub, mul, divi, modu,
	app, rot, shf, rev, sel, cut, ins,
	ptra, ptrs, ptr, len, fstF,
	push, pop, flip, unf,
	cmp, equ, Ce, Cs, Cg, Cn,
	firead, fiwrite, flen,
	rmr, jmp, rjmp,
	print, nprint, sprint,
	endprog
};

nry_t** stack;
nry_t** codex;
int64_t stackPtr;
int64_t codexPtr;
nry_t regs[regAmount];

// ######################################################################################## machine functions

bool pushtost(nry_t* src){
	stackPtr++;
	stack = realloc(stack, sizeof(nry_t*[stackPtr + 1]));
	if(stack == NULL){
		printf("\aFatal stack reallocation error on push.\n");
		return false;
	}
	stack[stackPtr] = malloc(sizeof(nry_t));
	makeimnry(stack[stackPtr], src);
	return true;
}

bool popfromst(nry_t* des){
	if(stackPtr < 0){
		printf("\aThere are no elements on the stack to pop.\n");
		return false;
	}
	copynry(des, stack[stackPtr]);
	freenry(stack[stackPtr]);
	free(stack[stackPtr]);
	stack = realloc(stack, sizeof(nry_t*[stackPtr]));
	stackPtr--;
	return true;
}

bool Flip(){
	if(stackPtr <= -1){
		printf("\aThere are no elements on the stack to flip.\n");
		return false;
	}

	codexPtr++;
	codex = realloc(codex, sizeof(nry_t*[codexPtr + 1]));
	if(codex == NULL){
		printf("\aFatal codex reallocation error on flip.\n");
		return false;
	}
	codex[codexPtr] = stack[stackPtr];
	stack = realloc(stack, sizeof(nry_t*[stackPtr]));
	stackPtr--;
	return true;
}

bool Unflip(){
	if(codexPtr <= -1){
		printf("\aThere are no elements on the codex to unflip.\n");
		return false;
	}

	stackPtr++;
	stack = realloc(stack, sizeof(nry_t*[stackPtr + 1]));
	if(stack == NULL){
		printf("\aFatal stack reallocation error on unflip.\n");
		return false;
	}
	stack[stackPtr] = codex[codexPtr];
	codex = realloc(codex, sizeof(nry_t[codexPtr]));
	codexPtr--;
	return true;
}

bool initmac(){
	bool bol = true;
	stackPtr = -1;
	stack = malloc(1);
	if(stack == NULL) bol &= false;
	codexPtr = -1;
	codex = malloc(1);
	if(codex == NULL) bol &= false;
	return bol;
}

void freemac(){
	for(; stackPtr > 0; stackPtr--){
		freenry(stack[stackPtr]);
		free(stack[stackPtr]);
	}
	free(stack);
	for(; codexPtr > 0; codexPtr--){
		freenry(codex[codexPtr]);
		free(codex[codexPtr]);
	}
	free(codex);
}

// ######################################################################################## execs

nry_t* exec(int ins, nry_t** args, bool* doprint){
	uint64_t dummy;
	FILE* quicfptr;
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
		case fstF*4 ... fstF*4 + 3: inttonry(args[0], args[1]->fst - args[1]->base, ins%4); break;

// push
		case push*4 ... push*4 + 3: *doprint &= pushtost(args[0]); break;
// pop
		case pop*4 ... pop*4 + 3: *doprint &= popfromst(args[0]); break;
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

// ######################################################################################## filing

typedef struct FILEstuff {
	uint64_t len;
	uint64_t pos;
	char* mfp;
} file_t;

char* mfgets(char* string, int size, file_t* file){
	int64_t i = 0;
	if(file->pos == file->len) return NULL;
	do {
		string[i] = file->mfp[file->pos + i];
		i++;
	} while(file->mfp[file->pos + i - 1] != '\n' && i <= size-1 && file->pos + i <= file->len);

	string[i] = '\0';
	file->pos += i;
//	printf("-------------------------------- %s\n", string);
	return string;
}

file_t* mfopen(char* path, file_t* file){
	FILE* fp = fopen(path, "r");
	if(fp == NULL){
		printf("\aCould not read file '%s'.\n", path);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	file->len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	file->mfp = malloc(file->len);
	fread(file->mfp, file->len, 1, fp);
	file->pos = 0;

	fclose(fp);
	return file;
}

void mfclose(file_t* file){
	free(file->mfp);
	file->mfp = NULL;
	file->pos = 0;
	file->len = 0;
}

void filerelexec(int ins, nry_t** args, file_t* file){
	switch(ins){
		case rmr*4 ... rmr*4 + 3: inttonry(args[0], file->pos, ins%4); break;
		case rjmp*4: case rjmp*4 + 2: file->pos = UN args[0]->fst; goto truerjmp;
		case rjmp*4 + 1: case rjmp*4 + 3: file->pos = UL args[0]->fst; goto truerjmp;
			truerjmp:
			inttonry(args[1], file->pos, ins%4);
			break;
	}
}

#endif

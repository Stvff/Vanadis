#ifndef VCO_H
#define VCO_H
#include "nry.h"
#include "iostr.h"

#define userInputLen 512
#define maxKeywordLen 16
#define argumentAmount 4

// ######################################################################################## instructions
char instructionString[][maxKeywordLen] = {
	"alloc", "free", "push", "pop", "flip", "unf", // 1
	"staptr", "cdxptr", // 2 
	"mov", "set", "mcpy", "rsz", // 3
	"inc", "dec", "add", "sub", "mul", "div", "mod", // 4
	"cmp", "equ", // 5
	"input", "printd", "print", "lib", // 6
	"fread", "fwrite", "flen", "time", // 7
	"ex", // 8
	"Ce", "Cs", "Cg", "Cn", "jmp", "call", "ret", // 9
	"bind", // 10
	"\0end"
};

enum instructionEnum {
	allocst, freest, push, pop, flip, unf, // 1
	staptr, cdxptr, // 2
	mov, set, mcpy, rsz, // 3
	inc, dec, add, sub, mul, divi, modu, // 4
	cmp, equ, // 5
	input, printd, print, lib, // 6
	firead, fiwrite, flen, timei, // 7
	ex, // 8
	Ce, Cs, Cg, Cn, jmp, call, ret, // 9
	bind, // 10
	final
};

char instructionKinds[][argumentAmount+1] = {
	"d___", "d___", "p___", "p___", "d___", "d___", // 1
	"d___", "d___", // 2
	"pp__", "dd__", "ppd_", "pd__", // 3
	"d___", "d___", "ddd_", "ddd_", "ddd_", "ddd_", "ddd_", // 4
	"dd__", "pp__", // 5
	"ppd_", "dd__", "pd__", "pppp", // 6
	"ppdd", "ppdd", "dpd_", "d___", // 7
	"d___" // 8
};

char operationString[] = {
	'$', '!', '^', ']', '>', '*', '*', 'l', 'o', 't', '~', ',', '%', 'N' };
enum operationEnum {
	opStackref, opImm, opMakenry, opEntry, opRef, opEntryKeep, opRefKeep,
	opLength, opOffset, opSizeof,
	opSwap, opComma, opNry, opNrs
};

char typeString[][4] = {
	"chr", "i8", "u8",
	"i16", "u16", "i32", "u32",
	"i64", "u64", "f32", "f64", "\0end"
};

// ######################################################################################## machine functions
uint8_t flag;
int STANDARDtype = I32;
int globalType;
nry_t** stack;
nry_t** codex;
int64_t stackPtr;
int64_t codexPtr;
int64_t stackFrameOffset;

time_t thetime;
char* UserInput;
uint64_t dummy;
uint64_t silly;
uint64_t giddy;
FILE* quicfptr;
file_t* quicmfptr;

bool stalloc(int64_t amount){
	if(amount < 0){
		printf("\aStack allocation amount can not be a negative value.\n");
		return false;
	} else if(amount == 0) amount = 1;
	stackPtr += amount;
	stack = realloc(stack, sizeof(nry_t*[stackPtr + 1]));
	if(stack == NULL){
		printf("\aFatal stack reallocation error on alloc.\n");
		return false;
	}
	for(int i = stackPtr-amount + 1; i < stackPtr + 1; i++){
		stack[i] = malloc(sizeof(nry_t));
//		printf("stackelptr: %lx\n", (uint64_t) stack[i]);
		makenry(stack[i], 8);
		memset(stack[i]->base, 0, 8);
	}
	return true;
}

bool stfree(int64_t amount){
	if(amount < 0){
		printf("\aStack free amount can not be a negative value.\n");
		return false;
	} else if(amount == 0) amount = 1;
	amount = stackPtr - amount;
	for(; stackPtr > -1 && stackPtr > amount; stackPtr--){
		freenry(stack[stackPtr]);
		free(stack[stackPtr]);
	}
	if(stackPtr == -1) stack = realloc(stack, 1);
	else stack = realloc(stack, sizeof(nry_t*[stackPtr + 1]));
	return true;
}

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
	stackFrameOffset = 0;
	stack = malloc(1);
	if(stack == NULL) bol &= false;
	codexPtr = -1;
	codex = malloc(1);
	if(codex == NULL) bol &= false;
	return bol;
}

void freemac(){
	for(; stackPtr >= 0; stackPtr--){
		freenry(stack[stackPtr]);
		free(stack[stackPtr]);
	}
	free(stack);
	for(; codexPtr >= 0; codexPtr--){
		freenry(codex[codexPtr]);
		free(codex[codexPtr]);
	}
	free(codex);
}

#endif

#ifndef VCO_H
#define VCO_H
#include "nry.h"
#include "iostr.h"

#define maxKeywordLen 16
#define STANDARDuserInputLen 512
#define argumentAmount 4

// ######################################################################################## instructions
char instructionString[][maxKeywordLen] = {
	"alloc", "free", "push", "pop", "flip", "unf", // 1
	"staptr", "cdxptr", // 2 
	"mov", "set", "mcpy", "rsz", // 3
	"inc", "dec", "add", "sub", "mul", "div", "mod", // 4
	"and", "or", "xor", "not", "rshf", "lshf", // 5
	"gc", "sc", "gec", "sec", "ec", "cmp", "pec", // 6
	"input", "dprint", "print", "lib", // 7
	"fread", "fwrite", "flen", "time", // 8
	"ex", // 9
	"Ce", "Cs", "Cg", "Cse", "Cge", "Cn", "jmp", "call", "ret", // 10
	"let", "enum", "import",// 12
	"\0end"
};

enum instructionEnum {
	allocst, freest, push, pop, flip, unf, // 1
	staptr, cdxptr, // 2
	mov, set, mcpy, rsz, // 3
	inc, dec, add, sub, mul, divi, modu, // 4
	and, or, xor, not, rshf, lshf, // 5
	gc, sc, gec, sec, ec, cmp, pec, // 6
	input, dprint, print, lib, // 7
	firead, fiwrite, flen, timei, // 8
	ex, // 9
	Ce, Cs, Cg, Cse, Cge, Cn, jmp, call, ret, // 10
	let, enumb, include,// 11
	final
};

char instructionKinds[][argumentAmount+1] = {
	"d___", "d___", "p___", "P___", "d___", "d___", // 1
	"D___", "D___", // 2
	"Pp__", "Dd__", "PPd_", "Pd__", // 3
	"D___", "D___", "Ddd_", "Ddd_", "Ddd_", "Ddd_", "Ddd_", // 4
	"Ddd_", "Ddd_", "Ddd_", "Dd__", "Ddd_", "Ddd_", // 5
	"dd__", "dd__", "dd__", "dd__", "dd__", "dd__", "pp__", // 6
	"Ppd_", "dd__", "pd__", "PPPp", // 7
	"PpdD", "ppdD", "DpD_", "D___", // 8
	"d___" // 9
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

enum compareEnum {
	CE = 1, CG = 2, CS = 4,
};

// ######################################################################################## machine functions
union {
	uint8_t s;
	struct {
		bool e:1;
		bool g:1;
		bool s:1;
	} c;
} flag;

int STANDARDtype = I32;
int globalType;
nry_t** stack;
nry_t** codex;
int64_t stackPtr;
int64_t codexPtr;
int64_t stackFrameOffset;

time_t thetime;
char* UserInput;
int userInputLen = STANDARDuserInputLen;
uint64_t dummy;
uint64_t silly;
uint64_t giddy;
FILE* quicfptr;
file_t* quicmfptr;

bool stalloc(int64_t amount){
	if(amount < 0){
		fprintf(stderr, "\aStack allocation amount can not be a negative value.\n");
		return false;
	} else if(amount == 0) amount = 1;
	stackPtr += amount;
	stack = realloc(stack, sizeof(nry_t*[stackPtr + 1]));
	if(stack == NULL){
		fprintf(stderr, "\aFatal stack reallocation error on alloc.\n");
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
		fprintf(stderr, "\aStack free amount can not be a negative value.\n");
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
		fprintf(stderr, "\aFatal stack reallocation error on push.\n");
		return false;
	}
	stack[stackPtr] = malloc(sizeof(nry_t));
	makeimnry(stack[stackPtr], src);
	return true;
}

bool popfromst(nry_t* des){
	if(stackPtr < 0){
		fprintf(stderr, "\aThere are no elements on the stack to pop.\n");
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
		fprintf(stderr, "\aThere are no elements on the stack to flip.\n");
		return false;
	}

	codexPtr++;
	codex = realloc(codex, sizeof(nry_t*[codexPtr + 1]));
	if(codex == NULL){
		fprintf(stderr, "\aFatal codex reallocation error on flip.\n");
		return false;
	}
	codex[codexPtr] = stack[stackPtr];
	stack = realloc(stack, sizeof(nry_t*[stackPtr]));
	stackPtr--;
	return true;
}

bool Unflip(){
	if(codexPtr <= -1){
		fprintf(stderr, "\aThere are no elements on the codex to unflip.\n");
		return false;
	}

	stackPtr++;
	stack = realloc(stack, sizeof(nry_t*[stackPtr + 1]));
	if(stack == NULL){
		fprintf(stderr, "\aFatal stack reallocation error on unflip.\n");
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

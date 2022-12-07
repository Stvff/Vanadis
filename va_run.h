#ifndef VA_RUN_H
#define VA_RUN_H
#include "util_nry.h"
#include "va_vic.h"
#include <time.h>
#include <math.h>

union {
	uint8_t s;
	struct {
		bool e:1;
		bool g:1;
		bool s:1;
	} c;
} flag;

bool debugEnters = false;
bool debugIns = false;
bool debugExpr = false;

typedef struct {
	int mglobalType;
	nry_t** mstack;
	nry_t** mcodex;
	int64_t mstackPtr;
	int64_t mcodexPtr;
	int64_t mstackFrameOffset;
	bool ret;
} VM;

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
		makenry(stack[i], 8);
		memset(stack[i]->base, 0, 8);
		if(debugIns) printnrydebug(stack[i]);
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

#ifndef libraryincluded
VM libraryfunctionexposedtoVanadis(VM themachine, nry_t** args, uint8_t** nrs){
	printf("No external libraries loaded. %p %p %p\n", &themachine, args, nrs);
	return themachine;
}
#else
VM libraryfunctionexposedtoVanadis(VM themachine, nry_t** args, uint8_t** nrs);
#endif

bool breakpoint(nry_t** pgst, uint8_t** nrst){
	yeah:
	printf("?> ");
	switch(fgetc(stdin)){
		case 'h':
		case '?':
			printf("i: toggle instruction info\n");
			printf("e: toggle expression info\n");
			printf("0: print top item of expression stack\n");
			printf("1: print other item in expression stack\n");
			printf("q: quit Vanadis\n");
			fgetc(stdin);
			goto yeah;
			break;
		case 'I': case 'i': debugIns = !debugIns; break;
		case 'E': case 'e': debugExpr = !debugExpr; break;
		case '0':
			if(nrst[0] != NULL){
				printf("----\n");
				printf("%lu\n", integer(nrst[0], globalType));
				printf("----\n");
			} else if(pgst[0] != NULL){
				printnrydebug(pgst[0]);
				aprintnry(pgst[0], U8, true);
				printf("----\n");
			}
			fgetc(stdin);
			goto yeah;
			break;
		case '1':
			if(nrst[1] != NULL){
				printf("----\n");
				printf("%lu\n", integer(nrst[1], globalType));
				printf("----\n");
			} else if(pgst[1] != NULL){
				printnrydebug(pgst[1]);
				aprintnry(pgst[1], U8, true);
				printf("---\n");
			}
			fgetc(stdin);
			goto yeah;
			break;
		case 'Q': case 'q': return false; break;
		case '\r': case '\n': return true; break;
	}
	fgetc(stdin);
	return true;
}


// ######################################################################################## execs
void stackerror(int64_t ref){
	fprintf(stderr, "\aInvalid stack reference.\n");
	fprintf(stderr, "stackPtr was %ld, stackFrameOffset was %ld, stack reference was %ld,\n", stackPtr, stackFrameOffset, ref);
	fprintf(stderr, "resulting in attempted read at %ld.\n", (int64_t) dummy);
}

bool evalexpr(char* expr, uint16_t exprlen, nry_t** args, uint8_t** nrs, nry_t* ALLp, uint64_t* ALLd){
	bool ret = true;
	uint16_t val = 0;
	nry_t* regp[2] = {NULL, NULL};
	uint8_t* regd[2] = {NULL, NULL};
	uint8_t argnr = 0;
	uint8_t and = 0;
	uint8_t anp = 0;
	if(debugExpr){
		printf("Solving expression now\n");
		printf("exprlen: x%x\n", exprlen);
	}
	for(uint16_t readhead = 2; readhead < exprlen; readhead++){
		val = u8 (expr + readhead);
		if(debugExpr){
			printf("p: %p, %p\nd: %p, %p\n", regp[0], regp[1], regd[0], regd[1]);
			printf("I %x: %c, readhead: %d\n", val, operationString[val], readhead);
			if(debugEnters) ret = breakpoint(regp, regd);
		}
		switch(val){
			case opNoop:
				fprintf(stderr, "\aThe expression opcode for a no-op was encountered. This should not happen, and is the fault of the compiler.\n");
				ret = false;
				break;
// stack
			case opStackref:
				dummy = stackFrameOffset + sinteger(regd[0], globalType);
				if((int64_t) dummy > stackPtr || (int64_t) dummy < 0){
					stackerror(sinteger(regd[0], globalType));
					ret = false; break;
				}
				regp[0] = stack[dummy];
				regd[0] = NULL;
				break;
// revstack
			case opStackrevref:
				dummy = stackPtr - sinteger(regd[0], globalType);
				if((int64_t) dummy > stackPtr || (int64_t) dummy < 0){
					stackerror(sinteger(regd[0], globalType));
					ret = false; break;
				}
				regp[0] = stack[dummy];
				regd[0] = NULL;
				break;
// immediate stack
			case opStackrefImm:
				dummy = stackFrameOffset + sinteger(regd[0], globalType);
				if((int64_t) dummy > stackPtr || (int64_t) dummy < 0){
					stackerror(sinteger(regd[0], globalType));
					ret = false; break;
				}
				regd[0] = stack[dummy]->fst;
				regp[0] = NULL;
				break;
// immediate revstack
			case opStackrevrefImm:
				dummy = stackPtr - sinteger(regd[0], globalType);
				if((int64_t) dummy > stackPtr || (int64_t) dummy < 0){
					stackerror(sinteger(regd[0], globalType));
					ret = false; break;
				}
				regd[0] = stack[dummy]->fst;
				regp[0] = NULL;
				break;
// immediate
			case opImm:
				regd[0] = regp[0]->fst;
				regp[0] = NULL;
				break;
// makenry
			case opMakenry:
				anp ^= 1;
				regp[0] = &ALLp[argnr*2 + anp];
				regp[0]->base = regd[0];
				regp[0]->fst = regp[0]->base;
				regp[0]->len = typeBylen(globalType);
				regd[0] = NULL;
				break;
// entry, direct
			case opEntry:
				if(regp[1]->len != 0) regd[0] = regp[1]->base + ((sinteger(regd[0], globalType)) % regp[1]->len);
				else regd[0] = regp[0]->base;
				regp[0] = NULL;
				break;
// entry, relative
			case opRef:
				if(regp[1]->len != 0) regd[0] = regp[1]->base + ((regp[1]->fst - regp[1]->base + sinteger(regd[0], globalType)) % regp[1]->len);
				else regd[0] = regp[0]->base;
				regp[0] = NULL;
				break;
// directAffect
			case opEntryKeep:
				if(regp[1]->len != 0) regp[1]->fst = regp[1]->base + ((sinteger(regd[0], globalType)) % regp[1]->len);
				else regp[1]->fst = regp[1]->base;
				regd[0] = regp[1]->fst;
				regp[0] = NULL;
				break;
// relativeAffect
			case opRefKeep:
				if(regp[1]->len != 0) regp[1]->fst = regp[1]->base + ((regp[1]->fst - regp[1]->base + sinteger(regd[0], globalType)) % regp[1]->len);
				else regp[1]->fst = regp[1]->base;
				regd[0] = regp[1]->fst;
				regp[0] = NULL;
				break;
// length
			case opLength:
				regd[0] = (uint8_t*) &regp[0]->len;
				if(globalType == F32) f32 regd[0] = (float) u64 regd[0];
				else if(globalType == F64) f64 regd[0] = (double) u64 regd[0];
				regp[0] = NULL;
				break;
// offset
			case opOffset:
				and ^= 1;
				regd[0] = (uint8_t*) &ALLd[argnr*2 + and];
				u64 regd[0] = regp[0]->fst - regp[0]->base;
				if(globalType == F32) f32 regd[0] = (float) u64 regd[0];
				else if(globalType == F64) f64 regd[0] = (double) u64 regd[0];
				regp[0] = NULL;
				break;
// sizeof
			case opSizeof:
				dummy = integer(regd[0], globalType);
				and ^= 1;
				regd[0] = (uint8_t*) &ALLd[argnr*2 + and];
				u64 regd[0] = dummy * typeBylen(globalType);
				if(globalType == F32) f32 regd[0] = (float) i64 regd[0];
				else if(globalType == F64) f64 regd[0] = (double) i64 regd[0];
				regp[0] = NULL;
				break;
// swap
			case opSwap:
				dummy = (uint64_t) regp[1];
				regp[1] = regp[0]; regp[0] = (nry_t*) dummy;
				dummy = (uint64_t) regd[1];
				regd[1] = regd[0]; regd[0] = (uint8_t*) dummy;
				break;
// comma
			case opComma:
				args[argnr] = regp[0];
				nrs[argnr] = regd[0];
				argnr++;
				and = 0;
				anp = 0;
				regp[0] = NULL; regp[1] = NULL; regd[0] = NULL; regd[1] = NULL;
				break;
// nry
			case opNry:
				readhead++;
				regp[1] = regp[0];
				and ^= 1;
				regp[0] = &ALLp[argnr*2 + and];
				regd[1] = regd[0];
				regd[0] = NULL;

				regp[0]->len = u16 (expr + readhead);
				readhead += 2;
				dummy = u16 (expr + readhead);
				readhead += 2;
				regp[0]->base = (uint8_t*) (expr + readhead);
				regp[0]->fst = regp[0]->base + dummy;
				readhead += regp[0]->len - 1;
				break;
// nrs
			case opNrs:
				readhead++;
				regp[1] = regp[0];
				regp[0] = NULL;
				regd[1] = regd[0];
				regd[0] = (uint8_t*) expr + readhead;
				readhead += typeBylen(globalType) - 1;
				break;
// brk
			case opBrk:
				ret = breakpoint(regp, regd);
				break;
		}
		if(!ret) break;
	}
	args[argnr] = regp[0];
	nrs[argnr] = regd[0];
	if(debugExpr){
		printf("p: %p, %p\nd: %p, %p\n", regp[0], regp[1], regd[0], regd[1]);
		printf("done solving expressions\n");
		if(debugEnters) ret = breakpoint(regp, regd);
	}
	return ret;
}

bool execute(char ins, nry_t** args, uint8_t** nrs){
	bool retbool = true;
	switch (ins) {
// alloc
		case allocst: retbool &= stalloc(integer(nrs[0], globalType)); break;
// free
		case freest: retbool &= stfree(integer(nrs[0], globalType)); break;
// push
		case push: retbool &= pushtost(args[0]); break;
// pop
		case pop: retbool &= popfromst(args[0]); break;
// flip
		case flip:
			dummy = integer(nrs[0], globalType);
			if(dummy == 0) dummy = 1;
			for(uint64_t i = 0; i < dummy; i++) if(!Flip()){ retbool &= false; break;};
			if(!retbool) break;
			break;			
// unf
		case unf:
			dummy = integer(nrs[0], globalType);
			if(dummy == 0) dummy = 1;
			for(uint64_t i = 0; i < dummy; i++) if(!Unflip()){ retbool &= false; break;};
			if(!retbool) break;
			break;

// staptr
		case staptr: switch(globalType){
			case Chr...U8: u8 nrs[0] = stackPtr; break;
			case I16: case U16: u16 nrs[0] = stackPtr; break;
			case I32: case U32: case F32: u32 nrs[0] = (float) stackPtr; break;
			case I64: case U64: case F64: u64 nrs[0] = (double) stackPtr; break;
		} break;
// cdxptr
		case cdxptr: switch(globalType){
			case Chr...U8: u8 nrs[0] = codexPtr; break;
			case I16: case U16: u16 nrs[0] = codexPtr; break;
			case I32: case U32: case F32: u32 nrs[0] = (float) codexPtr; break;
			case I64: case U64: case F64: u64 nrs[0] = (double) codexPtr; break;
		} break;

// mov
		case mov: copynry(args[0], args[1]); break;
// set
		case set: switch(globalType){
			case Chr...U8: u8 nrs[0] = u8 nrs[1]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1]; break;
			case I32: case U32: case F32: u32 nrs[0] = u32 nrs[1]; break;
			case I64: case U64: case F64: u64 nrs[0] = u64 nrs[1]; break;
		} break;
// cast
		case cast: switch( integer(nrs[2], globalType) ){
			case Chr: case I8: i8 nrs[0] = sinteger(nrs[1], globalType); break;
			case U8:   u8 nrs[0] = integer(nrs[1], globalType);  break;
			case I16: i16 nrs[0] = sinteger(nrs[1], globalType); break;
			case U16: u16 nrs[0] = integer(nrs[1], globalType);  break;
			case I32: i32 nrs[0] = sinteger(nrs[1], globalType); break;
			case U32: u32 nrs[0] = integer(nrs[1], globalType);  break;
			case I64: i64 nrs[0] = sinteger(nrs[1], globalType); break;
			case U64: u64 nrs[0] = integer(nrs[1], globalType);  break;
			case F32: f32 nrs[0] = float32(nrs[1], globalType);  break;
			case F64: f64 nrs[0] = float64(nrs[1], globalType);  break;
		} break;
// memv
		case memv:
			dummy = 1 + (args[0]->base + args[0]->len) - args[0]->fst;
			if(args[1]->len != 0) silly = 1 + args[1]->len - (integer(nrs[2], globalType) % args[1]->len);
			else silly = 1;
			if(silly < dummy) dummy = silly;
			silly = integer(nrs[2], globalType) % (1 + args[1]->len);
			memmove(args[0]->fst, args[1]->base + silly, integer(nrs[3], globalType)%dummy);
			break;
// fill
		case fill:
			dummy = integer(nrs[2], globalType) % (1 + (args[0]->base + args[0]->len) - args[0]->fst);
			memset(args[0]->fst, u8 nrs[1], dummy);
			break;
// rsz
		case rsz:
			dummy = integer(nrs[1], globalType);
			giddy = args[0]->fst - args[0]->base;
			if(dummy < giddy && dummy != 0) giddy %= dummy;
		  //else giddy = 0;
			if(dummy > args[0]->len){ silly = args[0]->len;} else silly = -1;
			remakenry(args[0], dummy);
			args[0]->fst = args[0]->base + giddy;
			if(silly != (uint64_t)-1) memset(args[0]->base + silly, 0, args[0]->len - silly);
			break;

// inc
		case inc: switch(globalType){
			case Chr...U8: (u8 nrs[0])++; break;
			case I16: case U16: (u16 nrs[0])++; break;
			case I32: case U32: (u32 nrs[0])++; break;
			case I64: case U64: (u64 nrs[0])++; break;
			case F32: (f32 nrs[0])++; break;
			case F64: (f64 nrs[0])++; break;
		} break;
// dec
		case dec: switch(globalType){
			case Chr...U8: (u8 nrs[0])--; break;
			case I16: case U16: (u16 nrs[0])--; break;
			case I32: case U32: (u32 nrs[0])--; break;
			case I64: case U64: (u64 nrs[0])--; break;
			case F32: (f32 nrs[0])--; break;
			case F64: (f64 nrs[0])--; break;
		} break;		
// add
		case add: switch(globalType){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] + u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] + u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] + u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] + u64 nrs[2]; break;
			case F32: f32 nrs[0] = f32 nrs[1] + f32 nrs[2]; break;
			case F64: f64 nrs[0] = f64 nrs[1] + f64 nrs[2]; break;
		} break;		
// sub
		case sub: switch(globalType){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] - u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] - u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] - u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] - u64 nrs[2]; break;
			case F32: f32 nrs[0] = f32 nrs[1] - f32 nrs[2]; break;
			case F64: f64 nrs[0] = f64 nrs[1] - f64 nrs[2]; break;
		} break;
// mul
		case mul: switch(globalType){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] * u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] * u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] * u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] * u64 nrs[2]; break;
			case F32: f32 nrs[0] = f32 nrs[1] * f32 nrs[2]; break;
			case F64: f64 nrs[0] = f64 nrs[1] * f64 nrs[2]; break;
		} break;
// div
		case divi: switch(globalType){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] / u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] / u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] / u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] / u64 nrs[2]; break;
			case F32: f32 nrs[0] = f32 nrs[1] / f32 nrs[2]; break;
			case F64: f64 nrs[0] = f64 nrs[1] / f64 nrs[2]; break;
		} break;		
// mod
		case modu: switch(globalType){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] % u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] % u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] % u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] % u64 nrs[2]; break;
			case F32: f32 nrs[0] = fmod(f32 nrs[1], f32 nrs[2]); break;
			case F64: f64 nrs[0] = fmod(f64 nrs[1], f64 nrs[2]); break;
		} break;

// and
		case and: switch(globalType){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] & u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] & u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] & u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] & u64 nrs[2]; break;
			case F32: f32 nrs[0] = u32 nrs[1] & u32 nrs[2]; break;
			case F64: f64 nrs[0] = u64 nrs[1] & u64 nrs[2]; break;
		} break;
// or
		case or: switch(globalType){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] | u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] | u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] | u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] | u64 nrs[2]; break;
			case F32: f32 nrs[0] = u32 nrs[1] | u32 nrs[2]; break;
			case F64: f64 nrs[0] = u64 nrs[1] | u64 nrs[2]; break;
		} break;
// xor
		case xor: switch(globalType){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] ^ u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] ^ u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] ^ u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] ^ u64 nrs[2]; break;
			case F32: f32 nrs[0] = u32 nrs[1] ^ u32 nrs[2]; break;
			case F64: f64 nrs[0] = u64 nrs[1] ^ u64 nrs[2]; break;
		} break;
// not
		case not: switch(globalType){
			case Chr...U8: u8 nrs[0] = !u8 nrs[1]; break;
			case I16: case U16: u16 nrs[0] = !u16 nrs[1]; break;
			case I32: case U32: u32 nrs[0] = !u32 nrs[1]; break;
			case I64: case U64: u64 nrs[0] = !u64 nrs[1]; break;
			case F32: f32 nrs[0] = !u32 nrs[1]; break;
			case F64: f64 nrs[0] = !u64 nrs[1]; break;
		} break;
// rshf
		case rshf: switch(globalType){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] >> u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] >> u8 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] >> u8 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] >> u8 nrs[2]; break;
			case F32: f32 nrs[0] = u32 nrs[1] >> u8 nrs[2]; break;
			case F64: f64 nrs[0] = u64 nrs[1] >> u8 nrs[2]; break;
		} break;
// lshf
		case lshf: switch(globalType){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] << u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] << u8 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] << u8 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] << u8 nrs[2]; break;
			case F32: f32 nrs[0] = u32 nrs[1] << u8 nrs[2]; break;
			case F64: f64 nrs[0] = u64 nrs[1] << u8 nrs[2]; break;
		} break;

// gcmp
		case gcmp: switch(globalType){
				case Chr: case I8: flag.c.g = i8 nrs[0] > i8 nrs[1]; break;
				case U8: flag.c.g = u8 nrs[0] > u8 nrs[1]; break;
				case I16: flag.c.g = i16 nrs[0] > i16 nrs[1]; break;
				case U16: flag.c.g = u16 nrs[0] > u16 nrs[1]; break;
				case I32: flag.c.g = i32 nrs[0] > i32 nrs[1]; break;
				case U32: flag.c.g = u32 nrs[0] > u32 nrs[1]; break;
				case I64: flag.c.g = i64 nrs[0] > i64 nrs[1]; break;
				case U64: flag.c.g = u64 nrs[0] > u64 nrs[1]; break;
				case F32: flag.c.g = f32 nrs[0] > f32 nrs[1]; break;
				case F64: flag.c.g = f64 nrs[0] > f64 nrs[1]; break;
			} break;
// scmp
		case scmp: switch(globalType){
				case Chr: case I8: flag.c.s = i8 nrs[0] < i8 nrs[1]; break;
				case U8: flag.c.s = u8 nrs[0] < u8 nrs[1]; break;
				case I16: flag.c.s = i16 nrs[0] < i16 nrs[1]; break;
				case U16: flag.c.s = u16 nrs[0] < u16 nrs[1]; break;
				case I32: flag.c.s = i32 nrs[0] < i32 nrs[1]; break;
				case U32: flag.c.s = u32 nrs[0] < u32 nrs[1]; break;
				case I64: flag.c.s = i64 nrs[0] < i64 nrs[1]; break;
				case U64: flag.c.s = u64 nrs[0] < u64 nrs[1]; break;
				case F32: flag.c.s = f32 nrs[0] < f32 nrs[1]; break;
				case F64: flag.c.s = f64 nrs[0] < f64 nrs[1]; break;
			} break;
// gecmp
		case gecmp: if(globalType < F32){
			switch(globalType){
				case Chr...U8: dummy = u8 nrs[0]; silly = u8 nrs[1]; break;
				case I16: case U16: dummy = u16 nrs[0]; silly = u16 nrs[1]; break;
				case I32: case U32: dummy = u32 nrs[0]; silly = u32 nrs[1]; break;
				case I64: case U64: dummy = u64 nrs[0]; silly = u64 nrs[1]; break;
			}
			if(globalType % 2 == 0)
				flag.s |= CE*(dummy == silly)
				        | CG*(dummy >  silly);
			else
				flag.s |= CE*((int64_t) dummy == (int64_t) silly)
				        | CG*((int64_t) dummy >  (int64_t) silly);
			} else switch(globalType){
				case F32:
					flag.s |= CE*(f32 nrs[0] == f32 nrs[1])
					        | CG*(f32 nrs[0] >  f32 nrs[1]); break;
				case F64:
					flag.s |= CE*(f64 nrs[0] == f64 nrs[1])
					        | CG*(f64 nrs[0] >  f64 nrs[1]); break;
			}; break;
// secmp
		case secmp: if(globalType < F32){
			switch(globalType){
				case Chr...U8: dummy = u8 nrs[0]; silly = u8 nrs[1]; break;
				case I16: case U16: dummy = u16 nrs[0]; silly = u16 nrs[1]; break;
				case I32: case U32: dummy = u32 nrs[0]; silly = u32 nrs[1]; break;
				case I64: case U64: dummy = u64 nrs[0]; silly = u64 nrs[1]; break;
			}
			if(globalType % 2 == 0)
				flag.s |= CE*(dummy == silly)
				        | CS*(dummy <  silly);
			else
				flag.s |= CE*((int64_t) dummy == (int64_t) silly)
				        | CS*((int64_t) dummy <  (int64_t) silly);				
			} else switch(globalType){
				case F32:
					flag.s |= CE*(f32 nrs[0] == f32 nrs[1])
					        | CS*(f32 nrs[0] <  f32 nrs[1]); break;
				case F64:
					flag.s |= CE*(f64 nrs[0] == f64 nrs[1])
					        | CS*(f64 nrs[0] <  f64 nrs[1]); break;
			}; break;
// ecmp
		case ecmp: switch(globalType){
				case Chr: case I8: flag.c.e = i8 nrs[0] == i8 nrs[1]; break;
				case U8: flag.c.e = u8 nrs[0] == u8 nrs[1]; break;
				case I16: flag.c.e = i16 nrs[0] == i16 nrs[1]; break;
				case U16: flag.c.e = u16 nrs[0] == u16 nrs[1]; break;
				case I32: flag.c.e = i32 nrs[0] == i32 nrs[1]; break;
				case U32: flag.c.e = u32 nrs[0] == u32 nrs[1]; break;
				case I64: flag.c.e = i64 nrs[0] == i64 nrs[1]; break;
				case U64: flag.c.e = u64 nrs[0] == u64 nrs[1]; break;
				case F32: flag.c.e = f32 nrs[0] == f32 nrs[1]; break;
				case F64: flag.c.e = f64 nrs[0] == f64 nrs[1]; break;
			}; break;
// cmp
		case cmp: if(globalType < F32){
			switch(globalType){
				case Chr...U8: dummy = u8 nrs[0]; silly = u8 nrs[1]; break;
				case I16: case U16: dummy = u16 nrs[0]; silly = u16 nrs[1]; break;
				case I32: case U32: dummy = u32 nrs[0]; silly = u32 nrs[1]; break;
				case I64: case U64: dummy = u64 nrs[0]; silly = u64 nrs[1]; break;
			}
			if(globalType % 2 == 0){
//				printf("%lu, %lu\n", dummy, silly);
				flag.s = CE*(dummy == silly)
				       | CS*(dummy <  silly)
				       | CG*(dummy >  silly);
			} else {
				flag.s = CE*((int64_t) dummy == (int64_t) silly)
				       | CS*((int64_t) dummy <  (int64_t) silly)
				       | CG*((int64_t) dummy >  (int64_t) silly);}
			} else switch(globalType){
				case F32:
					flag.s = CE*(f32 nrs[0] == f32 nrs[1])
					       | CS*(f32 nrs[0] <  f32 nrs[1])
					       | CG*(f32 nrs[0] >  f32 nrs[1]); break;
				case F64:
					flag.s = CE*(f64 nrs[0] == f64 nrs[1])
					       | CS*(f64 nrs[0] <  f64 nrs[1])
					       | CG*(f64 nrs[0] >  f64 nrs[1]); break;
			}; break;
// pecmp
		case pecmp:
			flag.s = CE*equalnry(args[0], args[1]);
			break;

// input
		case input: UserInput = malloc(userInputLen); switch(globalType){
			case Chr:
				dummy = 0;
				if(u8 nrs[2] == 0){
					fgets(UserInput, userInputLen, stdin);
					strcpytonry(args[0], UserInput);
				} else
					strtonry(args[0], (char*) args[1]->fst, (int*)&dummy);
				break;
			case I8 ... F64:
				remakenry(args[0], typeBylen(globalType));
				if(u8 nrs[2] == 0){
					fgets(UserInput, userInputLen, stdin);
					inputtonrs(args[0]->base, UserInput, 0, (int*)&dummy, globalType);
				} else
					inputtonrs(args[0]->base, (char*) args[1]->fst, 0, (int*)&dummy, globalType);
				break;
			} free(UserInput);
			break;
//printd
		case dprint: switch(globalType){
				case Chr: printf("%c", chr nrs[0]); break;
				case I8:  printf("%d", i8 nrs[0]); break;
				case U8:  printf("%u", u8 nrs[0]); break;
				case I16: printf("%d", i16 nrs[0]); break;
				case U16: printf("%u", u16 nrs[0]); break;
				case I32: printf("%d", i32 nrs[0]); break;
				case U32: printf("%u", u32 nrs[0]); break;
				case I64: printf("%ld", i64 nrs[0]); break;
				case U64: printf("%lu", u64 nrs[0]); break;
				case F32: printf("%f", f32 nrs[0]); break;
				case F64: printf("%lf", f64 nrs[0]); break;
			} if(u8 nrs[1] == 0) printf("\n");
			break;
// print
		case print: aprintnry(args[0], globalType, u8 nrs[1] == 0); break;

// lib
		case lib:
			(VM){globalType, stack, codex, stackPtr, codexPtr, stackFrameOffset, retbool}
			= libraryfunctionexposedtoVanadis(
			(VM){globalType, stack, codex, stackPtr, codexPtr, stackFrameOffset, retbool}
			, args, nrs);
			break;

// firead
		case firead:
			quicfptr = fopen((char*)(args[1]->fst), "rb");
			if(quicfptr == NULL){ i8 nrs[3] = -1; break;} i8 nrs[3] = 0;
			dummy = integer(args[0]->fst, globalType); freenry(args[0]); makenry(args[0], dummy);
			fseek(quicfptr, integer(nrs[2], globalType), SEEK_SET); fread(args[0]->base, 1, dummy, quicfptr);
			fclose(quicfptr); break;
// fwrite
		case fiwrite:
			if(u8 nrs[3] == 0) quicfptr = fopen((char*)(args[1]->fst), "wb"); else quicfptr = fopen((char*)(args[1]->fst), "wb+");
			if(quicfptr == NULL){ i8 nrs[3] = -1; break;} i8 nrs[3] = 0;
			fseek(quicfptr, integer(nrs[2], globalType), SEEK_SET); fwrite(args[0]->base, 1, args[0]->len, quicfptr);
			fclose(quicfptr); break;
// flen
		case flen:
			quicfptr = fopen((char*)(args[1]->fst), "r");
			if(quicfptr == NULL){ i8 nrs[2] = -1; break;} i8 nrs[2] = 0;
			fseek(quicfptr, 0, SEEK_END);
			dummy = ftell(quicfptr);
			switch(globalType){
				case Chr...U8: u8 nrs[0] = (uint8_t) dummy; break;
				case I16: case U16: u16 nrs[0] = (uint16_t) dummy; break;
				case I32: case U32: u32 nrs[0] = (uint32_t) dummy; break;
				case I64: case U64: u64 nrs[0] = (uint64_t) dummy; break;
				case F32: f32 nrs[0] = (float) dummy; break;
				case F64: f64 nrs[0] = (double) dummy; break;
			}
			fclose(quicfptr); break;

// time
		case timei:
			dummy = (uint64_t) time(&thetime);
			switch(globalType){
				case Chr...U8: u8 nrs[0] = (uint8_t) dummy; break;
				case I16: case U16: u16 nrs[0] = (uint16_t) dummy; break;
				case I32: case U32: u32 nrs[0] = (uint32_t) dummy; break;
				case I64: case U64: u64 nrs[0] = (uint64_t) dummy; break;
				case F32: f32 nrs[0] = (float) dummy; break;
				case F64: f64 nrs[0] = (double) dummy; break;
			}			
			break;
		case ex: break;
		default:
			fprintf(stderr, "\aAn unrecognized instruction opcode was encountered. This should not happen.\n");
			retbool = false;
			break;
	}
	return retbool;
}

#define exprbufflen 256
bool run(file_t* runfile){
	bool retbool = true;
	globalType = STANDARDtype;
	char head;
	bool insornot;
	uint16_t exprlen;
	nry_t callnr; makenry(&callnr, 16);

	nry_t* args[argumentAmount] = {0};
	uint8_t* nrs[argumentAmount] = {0};

	nry_t allp[argumentAmount*2] = {0};
	uint64_t alld[argumentAmount*2] = {0};

	while(runfile->pos < runfile->len && retbool){
		head = runfile->mfp[runfile->pos];
		runfile->pos++;
		insornot = head%2;
		head /= 2;
		if(insornot){
			if(debugIns)
				printf("Type is now %s\n", typeString[(signed char)head]);
			globalType = head;
		} else switch(head){
			case Ce: if(!flag.c.e) goto skip;;break;
			case Cs: if(!flag.c.s) goto skip; break;
			case Cg: if(!flag.c.g) goto skip; break;
			case Cse:if(!(flag.c.s || flag.c.e)) goto skip; break;
			case Cge:if(!(flag.c.g || flag.c.e)) goto skip; break;
			case Cn: if(flag.c.e) goto skip; break;
				skip: if(runfile->pos < runfile->len){
					head = runfile->mfp[runfile->pos]/2;
					if(head == call || head == jmp) runfile->pos += 9;
					else if(head == ret){ runfile->pos += 1; globalType = STANDARDtype;}
					else runfile->pos += (u16 (runfile->mfp + runfile->pos + 1)) + 1;
//					printf("skip, runfile->pos: %lx\n", runfile->pos);
				} break;
			case call:
				i64 callnr.base = stackFrameOffset;
				u64 (callnr.base + 8) = runfile->pos + 8;
				pushtost(&callnr); Flip();
				stackFrameOffset = stackPtr + 1;
//				printf("call, stackFrameOffset: x%lx, runfile->pos: %lx\n", stackFrameOffset, runfile->pos);
			case jmp:
//				globalType = STANDARDtype;
				runfile->pos = u64 (runfile->mfp + runfile->pos);
//				printf("jmp, runfile->pos: x%lx\n", runfile->pos);
				break;
			case ret:
				Unflip();
				popfromst(&callnr);
				stackFrameOffset = i64 callnr.base;
				runfile->pos = u64 (callnr.base + 8);
				globalType = STANDARDtype;
//				printf("ret: stackFrameOffset: x%lx, runfile->pos: %lx\n", stackFrameOffset, runfile->pos);
				break;
			default:
				if(debugIns)
					printf("default, ins: %s, x%x (multiplied by 2 in bin)\n", instructionString[(signed char)head], head);
				exprlen = u16 (runfile->mfp + runfile->pos);
				if(!evalexpr(runfile->mfp + runfile->pos, exprlen, args, nrs, allp, alld)){retbool = false; break;}
				if(!execute(head, args, nrs)){retbool = false; break;}
				runfile->pos += exprlen;
				break;
		}
		if(debugIns){
			printf("pos 0x%lx\n", runfile->pos);
			printf("stackPtr: %ld, stackFrameOffset: %ld\n", stackPtr, stackFrameOffset);
			if(debugEnters) retbool = breakpoint(NULL, NULL);
//			printstate();
		}
	}
	if(!retbool) printf("The code pointer was 0x%lx\n", runfile->pos);
	freenry(&callnr);
	return retbool;
}

#endif

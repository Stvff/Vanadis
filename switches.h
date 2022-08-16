#ifndef VINTERPRETER_H
#define VINTERPRETER_H
#include "nry.h"
#include "vic.h"
#include <time.h>
#include <math.h>

bool libraryfunctionexposedtoVanadis(nry_t** args){
	printf("%lx\n", (size_t)args);
	return true;
};

// ######################################################################################## execs

typedef struct{
	char freu[argumentAmount];
	bool err;
} rrr;

rrr evalexpr(char* expr, uint16_t exprlen, nry_t** args, uint8_t** nrs){
	rrr ret = {"----", true};
	uint16_t val = 0;
	nry_t* regp[2] = {NULL, NULL};
	uint8_t* regd[2] = {NULL, NULL};
	nry_t* allp[2] = {malloc(sizeof(nry_t)), malloc(sizeof(nry_t))}; allp[0]->base = NULL; allp[1]->base = NULL;
	uint8_t* alld[2] = {malloc(sizeof(uint64_t)), malloc(sizeof(uint64_t))};
//	printf("ap: %lx, %lx\nad: %lx, %lx\n", (uint64_t) allp[0], (uint64_t) allp[1], (uint64_t) alld[0], (uint64_t) alld[1]);
	uint8_t anr = 0;
	uint8_t argnr = 0;
//	printf("Solving expression now\n");
	for(uint16_t readhead = 2; readhead < exprlen; readhead++){
		val = u8 (expr + readhead);
//		printf("p: %lx, %lx\nd: %lx, %lx\n", (uint64_t) regp[0], (uint64_t) regp[1], (uint64_t) regd[0], (uint64_t) regd[1]);
//		printf("%x: %c\n", val, operationString[val]);
		switch(val){
// stack
			case opStackref: 
				dummy = stackFrameOffset + (int64_t)integer(regd[0], globalType);
				if((int64_t) dummy > stackPtr || (int64_t) dummy < 0){
					printf("\aInvalid stack reference.\n");
					printf("stackPtr was %ld, stackFrameOffset was %ld, stack reference was %ld,\n", stackPtr, stackFrameOffset, (int64_t)integer(regd[0], globalType));
					printf("resulting in attempted read at %ld.\n", (int64_t) dummy);
					ret.err = false; break;
				}
				regp[0] = stack[dummy];
				regd[0] = NULL;
				break;
// immediate
			case opImm:
				regd[0] = regp[0]->fst;
				regp[0] = NULL;
				break;
// makenry
			case opMakenry:
				regp[0] = allp[ regp[1]!=allp[0]?0:1 ];
				inttonry(regp[0], integer(regd[0], globalType), globalType);
				regd[0] = NULL;
				break;
// direct
			case opEntry:
				regd[0] = regp[1]->base + (((int64_t) integer(regd[0], globalType)) % regp[1]->len);
				regp[0] = NULL;
				break;
// relative
			case opRef:
				regd[0] = regp[1]->base + ((regp[1]->fst - regp[1]->base + (int64_t) integer(regd[0], globalType)) % regp[1]->len);
				regp[0] = NULL;
				break;
// directAffect
			case opEntryKeep:
				regp[1]->fst = regp[1]->base + (((int64_t) integer(regd[0], globalType)) % regp[1]->len);
				regd[0] = regp[1]->fst;
				regp[0] = NULL;
				break;
// relativeAffect
			case opRefKeep:
				regp[1]->fst = regp[1]->base + ((regp[1]->fst - regp[1]->base + (int64_t) integer(regd[0], globalType)) % regp[1]->len);
				regd[0] = regp[1]->fst;
				regp[0] = NULL;
				break;
// length
			case opLength:
				regd[0] = alld[ regd[1]!=alld[0]?0:1 ];
				u64 regd[0] = regp[0]->len;
				regp[0] = NULL;
				break;
// offset
			case opOffset:
				regd[0] = alld[ regd[1]!=alld[0]?0:1 ];
				u64 regd[0] = regp[0]->fst - regp[0]->fst;
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
				anr = regp[0]==allp[0]?0:(regp[0]==allp[1]?1:2);
				if(anr != 2){
					freenry(allp[1-anr]); free(allp[1-anr]);
					free(alld[0]);
					free(alld[1]);
					ret.freu[argnr] = 'p';
				} else {
					anr = regd[0]==alld[0]?0:(regd[0]==alld[1]?1:2);
					if(anr != 2){
						free(alld[1-anr]);
						freenry(allp[0]); free(allp[0]);
						freenry(allp[1]); free(allp[1]);
						ret.freu[argnr] = 'd';
					} else {
						freenry(allp[0]); free(allp[0]);
						freenry(allp[1]); free(allp[1]);
						free(alld[0]);
						free(alld[1]);
						ret.freu[argnr] = '_';
					}
				}
				args[argnr] = regp[0];
				nrs[argnr] = regd[0];
				allp[0] = malloc(sizeof(nry_t)); allp[1] = malloc(sizeof(nry_t)); allp[0]->base = NULL; allp[1]->base = NULL;
				alld[0] = malloc(sizeof(uint64_t)); alld[1] = malloc(sizeof(uint64_t));
				regp[0] = NULL; regp[1] = NULL; regd[0] = NULL; regd[1] = NULL;
				argnr++;
				break;
// nry
			case opNry:
				readhead++;
				regp[1] = regp[0];
				regp[0] = allp[ regp[1]!=allp[0]?0:1 ];
				regd[1] = regd[0];
				regd[0] = NULL;

				regp[0]->len = u16 (expr + readhead);
				readhead += 2;
				remakenry(regp[0], regp[0]->len);
				regp[0]->fst = regp[0]->base + u16 (expr + readhead);
				readhead += 2;
				memcpy(regp[0]->base, expr + readhead, regp[0]->len);
				readhead += regp[0]->len - 1;
				break;
// nrs
			case opNrs:
				readhead++;
				regp[1] = regp[0];
				regp[0] = NULL;
				regd[1] = regd[0];
				regd[0] = alld[ regd[1]!=alld[0]?0:1 ];

				u64 regd[0] = integer((uint8_t*)expr + readhead, globalType);
				readhead += typeBylen(globalType) - 1;
				break;
		}
		if(!ret.err) break;
	}
	anr = regp[0]==allp[0]?0:(regp[0]==allp[1]?1:2);
	if(anr != 2){
		freenry(allp[1-anr]); free(allp[1-anr]);
		free(alld[0]);
		free(alld[1]);
		ret.freu[argnr] = 'p';
	} else {
		anr = regd[0]==alld[0]?0:(regd[0]==alld[1]?1:2);
		if(anr != 2){
			free(alld[1-anr]);
			freenry(allp[0]); free(allp[0]);
			freenry(allp[1]); free(allp[1]);
			ret.freu[argnr] = 'd';
		} else {
			freenry(allp[0]); free(allp[0]);
			freenry(allp[1]); free(allp[1]);
			free(alld[0]);
			free(alld[1]);
			ret.freu[argnr] = '_';
		}
	}
	args[argnr] = regp[0];
	nrs[argnr] = regd[0];
//	printf("done solving expressions\n");
	return ret;
}


bool execute(char ins, nry_t** args, uint8_t** nrs){
	bool retbool = true;
	switch (ins) {
// alloc
		case allocst: stalloc(integer(nrs[0], globalType)); break;
// free
		case freest: stfree(integer(nrs[0], globalType)); break;
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

// mov
		case mov: copynry(args[0], args[1]); break;
// set
		case set: switch(globalType){
			case Chr...U8: u8 nrs[0] = u8 nrs[1]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1]; break;
			case I32: case U32: case F32: u32 nrs[0] = u32 nrs[1]; break;
			case I64: case U64: case F64: u64 nrs[0] = u64 nrs[1]; break;
		} break;
// mcpy
		case mcpy:
			dummy = (args[0]->base + args[0]->len) - args[0]->fst;
			silly = (args[1]->base + args[1]->len) - args[1]->fst;
			if(silly < dummy) dummy = silly;
			memmove(args[0]->fst, args[1]->fst, integer(nrs[2], globalType)%dummy);
			break;
// rsz
		case rsz:
			dummy = integer(nrs[1], globalType);
			giddy = (args[0]->fst - args[0]->base) % dummy;
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

// cmp
		case cmp: if(globalType < F32){
				if(integer(nrs[0], globalType) == integer(nrs[1], globalType)) flag = 0;
				else if(integer(nrs[0], globalType) < integer(nrs[1], globalType)) flag = 2;
				else flag = 1;
			} else if(globalType == F32){
				if(f32 nrs[0] == f32 nrs[1]) flag = 0;
				else if(f32 nrs[0] < f32 nrs[1]) flag = 2;
				else flag = 1;
			} else {
				if(f64 nrs[0] == f64 nrs[1]) flag = 0;
				else if(f64 nrs[0] < f64 nrs[1]) flag = 2;
				else flag = 1;
			} break;
// equ
		case equ:
			if(equalnry(args[0], args[1])) flag = 0;
			else flag = 3; ;break;

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
// print
		case print:
			if(u8 nrs[2] == 0) aprintnry(args[0], globalType, u8 nrs[1] == 0);
			else {switch(globalType){
				case Chr: printf("%c", chr args[0]->fst); break;
				case I8:  printf("%d", i8 args[0]->fst); break;
				case U8:  printf("%u", u8 args[0]->fst); break;
				case I16: printf("%d", i16 args[0]->fst); break;
				case U16: printf("%u", u16 args[0]->fst); break;
				case I32: printf("%d", i32 args[0]->fst); break;
				case U32: printf("%u", u32 args[0]->fst); break;
				case I64: printf("%ld", i64 args[0]->fst); break;
				case U64: printf("%lu", u64 args[0]->fst); break;
				case F32: printf("%f", f32 args[0]->fst); break;
				case F64: printf("%lf", f64 args[0]->fst); break;
			} if(u8 nrs[1] == 0) printf("\n");}
			break;

// lib
		case lib: retbool &= libraryfunctionexposedtoVanadis(args); break;

// firead
		case firead:
			quicfptr = fopen((char*)(args[1]->fst), "rb");
			if(quicfptr == NULL){ printf("\aCould not open file '%s'.\n", args[0]->fst); retbool &= false; break;}
			dummy = integer(args[0]->fst, globalType); freenry(args[0]); makenry(args[0], dummy);
			fseek(quicfptr, integer(nrs[2], globalType), SEEK_SET); fread(args[0]->base, 1, dummy, quicfptr);
			fclose(quicfptr); break;
// fwrite
		case fiwrite:
			if(u8 args[3]->fst == 0) quicfptr = fopen((char*)(args[1]->fst), "wb"); else quicfptr = fopen((char*)(args[1]->fst), "wb+");
			if(quicfptr == NULL){ printf("\aCould not open or create file '%s'.\n", args[1]->fst); retbool = false; break;}
			fseek(quicfptr, integer(nrs[2], globalType), SEEK_SET); fwrite(args[0]->base, 1, args[0]->len, quicfptr);
			fclose(quicfptr); break;
// flen
		case flen:
			quicfptr = fopen((char*)(args[1]->fst), "r");
			if(quicfptr == NULL){ printf("\aCould not read the length of file '%s'.\n", args[1]->fst); retbool = false; break;}
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
	}
	return retbool;
}

#endif

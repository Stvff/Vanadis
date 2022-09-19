#ifndef VINTERPRETER_H
#define VINTERPRETER_H
#include "nry.h"
#include "vic.h"
#include <time.h>
#include <math.h>

#ifndef libraryincluded
bool libraryfunctionexposedtoVanadis(nry_t** args){
	printf("hello from only this side\n");
	return true;
}
#else
bool libraryfunctionexposedtoVanadis(nry_t** args);
#endif

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
//	printf("Solving expression now\n");
//	printf("exprlen: x%x\n", exprlen);
	for(uint16_t readhead = 2; readhead < exprlen; readhead++){
		val = u8 (expr + readhead);
//		printf("p: %lx, %lx\nd: %lx, %lx\n", (uint64_t) regp[0], (uint64_t) regp[1], (uint64_t) regd[0], (uint64_t) regd[1]);
//		printf("in %x: %c, readhead: %d\n", val, operationString[val], readhead);
		switch(val){
			case opNoop:
				fprintf(stderr, "\aNo operation! This is a problem!\n");
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
// direct
			case opEntry:
				regd[0] = regp[1]->base + ((sinteger(regd[0], globalType)) % regp[1]->len);
				regp[0] = NULL;
				break;
// relative
			case opRef:
				regd[0] = regp[1]->base + ((regp[1]->fst - regp[1]->base + sinteger(regd[0], globalType)) % regp[1]->len);
				regp[0] = NULL;
				break;
// directAffect
			case opEntryKeep:
				regp[1]->fst = regp[1]->base + ((sinteger(regd[0], globalType)) % regp[1]->len);
				regd[0] = regp[1]->fst;
				regp[0] = NULL;
				break;
// relativeAffect
			case opRefKeep:
				regp[1]->fst = regp[1]->base + ((regp[1]->fst - regp[1]->base + sinteger(regd[0], globalType)) % regp[1]->len);
				regd[0] = regp[1]->fst;
				regp[0] = NULL;
				break;
// length
			case opLength:
				regd[0] = (uint8_t*) &regp[0]->len;
				regp[0] = NULL;
				break;
// offset
			case opOffset:
				and ^= 1;
				regd[0] = (uint8_t*) &ALLd[argnr*2 + and];
				u64 regd[0] = regp[0]->fst - regp[0]->base;
				regp[0] = NULL;
				break;
// sizeof
			case opSizeof:
				dummy = integer(regd[0], globalType);
				and ^= 1;
				regd[0] = (uint8_t*) &ALLd[argnr*2 + and];
				u64 regd[0] = dummy * typeBylen(globalType);
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
		}
		if(!ret) break;
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
// memv
		case memv:
			dummy = (args[0]->base + args[0]->len) - args[0]->fst;
			silly = (args[1]->base + args[1]->len) - args[1]->fst;
			if(silly < dummy) dummy = silly;
			silly = integer(nrs[2], globalType) % args[0]->len;
			memmove(args[0]->fst, args[1]->base + silly, integer(nrs[3], globalType)%dummy);
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

// gc
		case gc: switch(globalType){
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
// sc
		case sc: switch(globalType){
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
// gec
		case gec: if(globalType < F32){
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
			} break;
// sec
		case sec: if(globalType < F32){
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
			} break;
// ec
		case ec: switch(globalType){
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
			} break;
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
			} break;
// pec
		case pec:
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
		case lib: retbool &= libraryfunctionexposedtoVanadis(args); break;

// firead
		case firead:
			quicfptr = fopen((char*)(args[1]->fst), "rb");
			if(quicfptr == NULL){ i8 nrs[3] = -1; break;} i8 nrs[3] = 0;
			dummy = integer(args[0]->fst, globalType); freenry(args[0]); makenry(args[0], dummy);
			fseek(quicfptr, integer(nrs[2], globalType), SEEK_SET); fread(args[0]->base, 1, dummy, quicfptr);
			fclose(quicfptr); break;
// fwrite
		case fiwrite:
			if(u8 args[3]->fst == 0) quicfptr = fopen((char*)(args[1]->fst), "wb"); else quicfptr = fopen((char*)(args[1]->fst), "wb+");
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
	}
	return retbool;
}

#endif

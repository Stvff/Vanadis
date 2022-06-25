#ifndef VINTERPRETER_H
#define VINTERPRETER_H
#include "vic.h"
#include <time.h>
#include <math.h>

// ######################################################################################## execs
nry_t* exec(int ins, int type, nry_t** args, uint8_t** nrs, bool* doprint){
	nry_t* retptr = args[0];
	switch (ins) {
// set
		case mov: copynry(args[0], args[1]); break;
// set
		case set: switch(type){
			case Chr...U8: u8 nrs[0] = u8 nrs[1]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1]; break;
			case I32: case U32: case F32: u32 nrs[0] = u32 nrs[1]; break;
			case I64: case U64: case F64: u64 nrs[0] = u64 nrs[1]; break;
		} break;

// inc
		case inc: switch(type){
			case Chr...U8: (u8 nrs[0])++; break;
			case I16: case U16: (u16 nrs[0])++; break;
			case I32: case U32: (u32 nrs[0])++; break;
			case I64: case U64: (u64 nrs[0])++; break;
			case F32: (f32 nrs[0])++; break;
			case F64: (f64 nrs[0])++; break;
		} break;
// dec
		case dec: switch(type){
			case Chr...U8: (u8 nrs[0])--; break;
			case I16: case U16: (u16 nrs[0])--; break;
			case I32: case U32: (u32 nrs[0])--; break;
			case I64: case U64: (u64 nrs[0])--; break;
			case F32: (f32 nrs[0])--; break;
			case F64: (f64 nrs[0])--; break;
		} break;
		
// add
		case add: switch(type){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] + u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] + u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] + u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] + u64 nrs[2]; break;
			case F32: f32 nrs[0] = f32 nrs[1] + f32 nrs[2]; break;
			case F64: f64 nrs[0] = f64 nrs[1] + f64 nrs[2]; break;
		} break;		
// sub
		case sub: switch(type){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] - u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] - u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] - u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] - u64 nrs[2]; break;
			case F32: f32 nrs[0] = f32 nrs[1] - f32 nrs[2]; break;
			case F64: f64 nrs[0] = f64 nrs[1] - f64 nrs[2]; break;
		} break;

// mul
		case mul: switch(type){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] * u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] * u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] * u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] * u64 nrs[2]; break;
			case F32: f32 nrs[0] = f32 nrs[1] * f32 nrs[2]; break;
			case F64: f64 nrs[0] = f64 nrs[1] * f64 nrs[2]; break;
		} break;
// div
		case divi: switch(type){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] / u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] / u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] / u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] / u64 nrs[2]; break;
			case F32: f32 nrs[0] = f32 nrs[1] / f32 nrs[2]; break;
			case F64: f64 nrs[0] = f64 nrs[1] / f64 nrs[2]; break;
		} break;		
// mod
		case modu: switch(type){
			case Chr...U8: u8 nrs[0] = u8 nrs[1] % u8 nrs[2]; break;
			case I16: case U16: u16 nrs[0] = u16 nrs[1] % u16 nrs[2]; break;
			case I32: case U32: u32 nrs[0] = u32 nrs[1] % u32 nrs[2]; break;
			case I64: case U64: u64 nrs[0] = u64 nrs[1] % u64 nrs[2]; break;
			case F32: f32 nrs[0] = fmod(f32 nrs[1], f32 nrs[2]); break;
			case F64: f64 nrs[0] = fmod(f64 nrs[1], f64 nrs[2]); break;
		} break;

// push
		case push: *doprint &= pushtost(args[0]); break;
// pop
		case pop: *doprint &= popfromst(args[0]); break;
// peek
		case peek: if(stackPtr == -1){ printf("\aThere are no elements on the stack\n"); *doprint = false; break;}
			copynry(args[0], stack[stackPtr]); break;
// flip
		case flip:
			dummy = integer(nrs[1], type);
			if(dummy == 0) dummy = 1;
			for(uint64_t i = 0; i < dummy; i++) if(!Flip()){ *doprint &= false; dummy = -117; break;};
			if(dummy == (uint64_t) -117) break;
			retptr = codex[codexPtr];
			break;			
// unf
		case unf:
			dummy = integer(nrs[1], type);
			if(dummy == 0) dummy = 1;
			for(uint64_t i = 0; i < dummy; i++) if(!Unflip()){ *doprint &= false; dummy = -117; break;};
			if(dummy == (uint64_t) -117) break;
			retptr = stack[stackPtr];
			break;

// cmp
		case cmp: if(type < F32){
				if(integer(nrs[0], type) == integer(nrs[1], type)) flag = 0;
				else if(integer(nrs[0], type) < integer(nrs[1], type)) flag = 2;
				else flag = 1;
			} else if(type == F32){
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

		case input: switch(type){
			case Chr:
				dummy = 0; if(u8 args[2]->fst == 0){
				fgets(UserInput, userInputLen, stdin);
				strcpytonry(args[0], UserInput);} else
				strtonry(args[0], (char*) args[1]->fst, (int*)&dummy);
				break;
			case I8 ... F64:
				remakenry(args[0], typeBylen(type));
				if(u8 args[2]->fst == 0){
					fgets(UserInput, userInputLen, stdin);
					inputtonrs(args[0]->base, UserInput, 0, (int*)&dummy, type);
				}
				else inputtonrs(args[0]->base, (char*) args[1]->fst, 0, (int*)&dummy, type);
				break;
			}break;

// print
		case print:
			if(u8 nrs[2] == 0) aprintnry(args[0], ins%4, u8 nrs[1] == 0);
			else switch(type){
				case Chr: printf("%c", chr args[0]->fst); break;
				case I8: printf("%d", i8 args[0]->fst); break;
				case U8: printf("%u", u8 args[0]->fst); break;
				case I16: printf("%d", i16 args[0]->fst); break;
				case U16: printf("%u", u16 args[0]->fst); break;
				case I32: printf("%d", i32 args[0]->fst); break;
				case U32: printf("%u", u32 args[0]->fst); break;
				case I64: printf("%ld", i64 args[0]->fst); break;
				case U64: printf("%lu", u64 args[0]->fst); break;
				case F32: printf("%f", f32 args[0]->fst); break;
				case F64: printf("%lf", f64 args[0]->fst); break;
			} if(u8 nrs[1] == 0) printf("\n");
			break;

// firead
		case firead:
			quicfptr = fopen((char*)(args[1]->fst), "rb");
			if(quicfptr == NULL){ printf("\aCould not open file '%s'.\n", args[0]->fst); *doprint &= false; break;}
			dummy = integer(args[0]->fst, type); freenry(args[0]); makenry(args[0], dummy);
			fseek(quicfptr, integer(args[2]->fst, type), SEEK_SET); fread(args[0]->base, 1, dummy, quicfptr);
			fclose(quicfptr); break;

// fwrite
		case fiwrite:
			if(u8 args[3]->fst == 0) quicfptr = fopen((char*)(args[1]->fst), "wb"); else quicfptr = fopen((char*)(args[1]->fst), "wb+");
			if(quicfptr == NULL){ printf("\aCould not open or create file '%s'.\n", args[1]->fst); doprint = false; break;}
			fseek(quicfptr, integer(args[2]->fst, type), SEEK_SET); fwrite(args[0]->base, 1, args[0]->len, quicfptr);
			fclose(quicfptr); break;

// flen
		case flen:
			quicfptr = fopen((char*)(args[1]->fst), "r");
			if(quicfptr == NULL){ printf("\aCould not read the length of file '%s'.\n", args[1]->fst); doprint = false; break;}
			fseek(quicfptr, 0, SEEK_END); inttonry(args[0], ftell(quicfptr), ins%4); fclose(quicfptr); break;
	}
	return retptr;
}

void filerelexec(int ins, uint8_t** nrs, file_t** filepp){
	file_t* file = *filepp;
	switch(ins){
		case jmp:
			globalType = STANDARDtype;
			file->pos = file->labelposs[u64 nrs[0]];
			break;
		case call:
			break;
		case lcall:
			break;
		case syscall:
			break;
/*			dummy = 0;
			dummy = keywordlook((char*)args[0]->fst, labelSize, file->labels, &dummy);
			if(dummy == -1){ printf("\aLabel '%s' does not exist.\n", args[1]->fst); break;}
			inttonry(args[1], file->pos, ins%4);
			file->pos = file->labelposs[dummy];
			break;
//		case run:
//			*filepp = openscript((char*)args[0]->fst, file);
			break;*/
	}
}

#endif

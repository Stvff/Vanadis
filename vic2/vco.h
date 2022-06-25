#ifndef VCO_H
#define VCO_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define chr *(char*)
#define i8 *(int8_t*)
#define u8 *(uint8_t*)
#define i16 *(int16_t*)
#define u16 *(uint16_t*)
#define i32 *(int32_t*)
#define u32 *(uint32_t*)
#define i64 *(int64_t*)
#define u64 *(uint64_t*)
#define f32 *(float*)
#define f64 *(double*)

enum datatypes {Chr = 0, I8, U8, I16, U16, I32, U32, I64, U64, F32, F64};

bool EndLine(char* entry){
	return *entry == '\n' || *entry == '\r' || *entry == '\0' || *entry == ';' || *entry == ':' || *entry == '#';
}

bool IsNr(char* entry){
	return (*entry >= '0' && *entry <= '9') || *entry == '-';
}

bool IsAlph(char* entry){
	return (*entry >= 'a' && *entry <= 'z') || (*entry >= 'A' && *entry <= 'Z');
}

bool IsSpace(char* entry){
	return *entry == ' ' || *entry == '\t';
}

int typeBylen(int type){
	switch(type){
		case Chr...U8: return 1; break;
		case I16: case U16: return 2; break;
		case I32: case U32: case F32: return 4; break;
		case I64: case U64: case F64: return 8; break;
	}
}

uint64_t integer(uint8_t* ptr, int type){
	switch(type){
		case Chr...U8: return u8 ptr; break;
		case I16: case U16: return u16 ptr; break;
		case I32: case U32: return u32 ptr; break;
		case I64: case U64: return u64 ptr; break;
		case F32: return (uint64_t) f32 ptr; break;
		case F64: return (uint64_t) f64 ptr; break;
	}
}

// ######################################################################################## nry stuff
typedef struct ARRAy {
	uint8_t* base;
	uint64_t len;
	uint8_t* fst;
} nry_t;

nry_t* makenry(nry_t* des, uint64_t len){
	des->base = malloc(len + 8);
	memset(des->base + len, 0, 8);
	des->fst = des->base;
	des->len = len;
	return des;
}

nry_t* freenry(nry_t* des){
	free(des->base);
	des->base = NULL;
	des->fst = NULL;
	des->len = 0;
	return des;
}

nry_t* remakenry(nry_t* des, uint64_t len){
	des->base = realloc(des->base, len + 8);
	des->fst = des->base;
	memset(des->base + len, 0, 8);
	des->len = len;
	return des;
}

nry_t* makeimnry(nry_t* des, nry_t* src){
	des->base = malloc(src->len + 8);
	des->len = src->len;
	memset(des->base + des->len, 0, 8);
	des->fst = des->base + (src->fst - src->base);
	des->base = memcpy(des->base, src->base, des->len);	
	return des;
}

nry_t* copynry(nry_t* des, nry_t* src){
	if(des->base == src->base) return des;
	des->base = realloc(des->base, src->len + 8);
	des->len = src->len;
	memset(des->base + des->len, 0, 8);
	des->fst = des->base + (src->fst - src->base);
	des->base = memcpy(des->base, src->base, des->len);
	return des;
}

nry_t* appendnry(nry_t* des, nry_t* src){
	uint64_t off = des->fst - des->base;
	des->base = realloc(des->base, des->len + src->len + 8);
	des->fst = des->base + off;
	memcpy(des->base + des->len, src->base, src->len);
	des->len += src->len;
	memset(des->base + des->len, 0, 8);
	return des;
}

nry_t* inttonry(nry_t* des, uint64_t inte, int type){
	remakenry(des, typeBylen(type));
	switch(type){
		case Chr...U8: u8 des->base = (uint8_t) inte; break;
		case I16: case U16: u16 des->base = (uint16_t) inte; break;
		case I32: case U32: u32 des->base = (uint32_t) inte; break;
		case I64: case U64:  u64 des->base = inte; break;
		case F32: f32 des->base = f32 &inte; break;
		case F64: f64 des->base = f64 &inte; break;
	}
	return des;
}

nry_t* strcpytonry(nry_t* des, char* str){
	uint64_t i = 0;
	while(str[i] != '\0'){
		i++;
	}
	remakenry(des, i);
	memcpy(des->base, str, i);
	return des;
}

nry_t* strtonry(nry_t* des, char* str, int* start){
	int i = *start;
	uint64_t j = 0;
	while(str[i] != '"' && str[i] != '\0'){
		if(str[i] == '\\') i++;
		i++; j++;
	}
	remakenry(des, j);

	j = 0;
	int n;
	for(n = *start; n < i; n++){
		if(str[n] == '\\'){
			switch (str[n+1]){
				case '0': des->base[j] = '\0'; break;
				case 'a': des->base[j] = '\a'; break;
				case 'b': des->base[j] = '\b'; break;
				case 't': des->base[j] = '\t'; break;
				case 'v': des->base[j] = '\v'; break;
				case 'f': des->base[j] = '\f'; break;
				case 'r': des->base[j] = '\r'; break;
				case 'n': des->base[j] = '\n'; break;
				case '\\': des->base[j] = '\\'; break;
				case '\"': des->base[j] = '\"'; break;
			}
			n++;
		} else des->base[j] = str[n];
		j++;
	}
	*start = n + 1;
	return des;
}

uint64_t inputtoint(char* input, int start, int* end){
	int i = start;
	char entry = input[i];
	while (IsNr(&entry) || entry == '-' || entry == ' ')
		entry = input[++i];
	*end = i - 1;
	i--;
	uint64_t pow = 1;
	uint64_t accum = 0;
	bool negative = false;
	for( ; i >= start; i--){
		entry = input[i];
		if(entry == '-') negative = !negative;
		else if(entry != ' '){
			accum += (entry - 0x30)*pow;
			pow *= 10;
		}
	}
	if(negative) accum *= -1;
	return accum;
}

uint8_t* inputtonrs(uint8_t* des, char* input, int start, int* end, int type){
	uint64_t nr = inputtoint(input, start, end);
	switch(type){
		case I8...U8: u8 des = (uint8_t) nr; break;
		case I16...U16: u16 des = (uint16_t) nr; break;
		case I32...U32: u32 des = (uint32_t) nr; break;
		case I64...U64: u64 des = (uint64_t) nr; break;
		case F32: f32 des = (float) (int64_t) nr; break;
		case F64: f64 des = (double) (int64_t) nr; break;
	}
	return des;
}

void aprintnry(nry_t* src, int type, bool endline){
	if(type == Chr){
		src->base[src->len] = '\0';
		printf("%s", src->fst);
		goto endoffunc;
	}
	int inc = typeBylen(type);
	if(src->len == 0){ printf("'"); goto endoffunc;}
	for(uint64_t i = 0; i < src->len; i += inc) {
		if(src->base + i == src->fst) printf("'");
		switch(type){
			case I8: printf("%d + ", i8 (src->base + i)); break;
			case U8: printf("%u + ", u8 (src->base + i)); break;
			case I16: printf("%d + ", i16 (src->base + i)); break;
			case U16: printf("%u + ", u16 (src->base + i)); break;
			case I32: printf("%d + ", i32 (src->base + i)); break;
			case U32: printf("%u + ", u32 (src->base + i)); break;
			case I64: printf("%ld + ", i64 (src->base + i)); break;
			case U64: printf("%lu + ", u64 (src->base + i)); break;
			case F32: printf("%f + ", f32 (src->base + i)); break;
			case F64: printf("%lf + ", f64 (src->base + i)); break;
		};
	}
	printf("\b\b\b   \b\b\b");
	endoffunc:
	if(endline) printf("\n");
}

bool equalnry(nry_t* a, nry_t* b){
	if(a->len != b->len) return false;
	for(uint64_t i = 0; i < a->len; i+=8)
		if(u64 (a->base + i) != u64 (b->base + i)) return false;
	return true;
}

// ######################################################################################## machine functions
#define userInputLen 256
#define maxKeywordLen 16
#define argumentAmount 4

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
FILE* quicfptr;

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
	for(int i = stackPtr-amount + 1; i < stackPtr; i++){
		makenry(stack[i], 8);
	}
	return true;
}

bool stfree(int64_t amount){
	if(amount < 0){
		printf("\aStack free amount can not be a negative value.\n");
		return false;
	} else if(amount == 0) amount = 1;
	amount = stackPtr - amount;
	for(; stackPtr > 0 && stackPtr > amount; stackPtr--){
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

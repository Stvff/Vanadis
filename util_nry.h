#ifndef UTIL_NRY_H
#define UTIL_NRY_H
#include "util_common.h"

typedef union {
	char* chr;
	int8_t* i8;
	uint8_t* u8;
	int16_t* i16;
	uint16_t* u16;
	int32_t* i32;
	uint32_t* u32;
	int64_t* i64;
	uint64_t* u64;
	float* f32;
	double* f64;
	void* p;
} ptr_t;

enum datatypes {Chr = 0, I8, U8, I16, U16, I32, U32, I64, U64, F32, F64};
//                    0   1   2    3    4    5    6    7    8    9   10

int8_t typelen[11] = { 1, 1,  1,  2,   2,   4,   4,   8,   8,   4,   8};

/*int typeBylen(int type){
	switch(type){
		case Chr...U8: return 1; break;
		case I16: case U16: return 2; break;
		case I32: case U32: case F32: return 4; break;
		case I64: case U64: case F64: return 8; break;
	}
	return 1;
}*/

uint64_t integer(ptr_t ptr, int type){
	switch(type){
		case Chr...U8: return *ptr.u8; break;
		case I16: case U16: return *ptr.u16; break;
		case I32: case U32: return *ptr.u32; break;
		case I64: case U64: return *ptr.u64; break;
		case F32: return (uint64_t) *ptr.f32; break;
		case F64: return (uint64_t) *ptr.f64; break;
	}
	return 0;
}

int64_t sinteger(ptr_t ptr, int type){
	switch(type){
		case Chr...U8: return *ptr.i8; break;
		case I16: case U16: return *ptr.i16; break;
		case I32: case U32: return *ptr.i32; break;
		case I64: case U64: return *ptr.i64; break;
		case F32: return (int64_t) *ptr.f32; break;
		case F64: return (int64_t) *ptr.f64; break;
	}
	return 0;
}

float float32(ptr_t ptr, int type){
	switch(type){
		case Chr...U8: return (float) *ptr.u8; break;
		case I16: case U16: return (float) *ptr.u16; break;
		case I32: case U32: return (float) *ptr.u32; break;
		case I64: case U64: return (float) *ptr.u64; break;
		case F32: return *ptr.f32; break;
		case F64: return (float) *ptr.f64; break;
	}
	return 0;
}

double float64(ptr_t ptr, int type){
	switch(type){
		case Chr...U8: return (double) *ptr.u8; break;
		case I16: case U16: return (double) *ptr.u16; break;
		case I32: case U32: return (double) *ptr.u32; break;
		case I64: case U64: return (double) *ptr.u64; break;
		case F32: return (double) *ptr.f32; break;
		case F64: return *ptr.f64; break;
	}
	return 0;
}

// ######################################################################################## nry stuff
typedef struct {
	ptr_t base;
	size_t len;
	ptr_t fst;
} nry_t;

nry_t* makenry(nry_t* des, uint64_t len){
	des->base.p = malloc(len + 8);
	if(des->base.p == NULL) printf("WHAT???? HOW???? WHAT NOW??? (makenry)\n");
	memset(des->base.u8 + len, 0, 8);
	des->fst = des->base;
	des->len = len;
	return des;
}

nry_t* freenry(nry_t* des){
	free(des->base.p);
	des->base.p = NULL;
	des->fst.p = NULL;
	des->len = 0;
	return des;
}

nry_t* remakenry(nry_t* des, size_t len){
	des->base.p = realloc(des->base.p, len + 8);
	if(des->base.p == NULL) printf("WHAT???? HOW???? WHAT NOW??? (remakenry)\n");
	des->fst = des->base;
	memset(des->base.u8 + len, 0, 8);
	des->len = len;
	return des;
}

nry_t* makeimnry(nry_t* des, nry_t* src){
	des->base.p = malloc(src->len + 8);
	if(des->base.p == NULL) printf("WHAT???? HOW???? WHAT NOW??? (makeimnry)\n");
	des->len = src->len;
	memset(des->base.u8 + des->len, 0, 8);
	des->fst.u8 = des->base.u8 + (src->fst.u8 - src->base.u8);
	des->base.p = memcpy(des->base.p, src->base.p, des->len);
	return des;
}

nry_t* copynry(nry_t* des, nry_t* src){
	if(des->base.p == src->base.p) return des;
	des->base.p = realloc(des->base.p, src->len + 8);
	if(des->base.p == NULL) printf("WHAT???? HOW???? WHAT NOW??? (copynry)\n");
	des->len = src->len;
	memset(des->base.u8 + des->len, 0, 8);
	des->fst.u8 = des->base.u8 + (src->fst.u8 - src->base.u8);
	des->base.p = memcpy(des->base.p, src->base.p, des->len);
	return des;
}

nry_t* inttonry(nry_t* des, uint64_t inte, int type){
	remakenry(des, typelen[type]);
	switch(type){
		case Chr...U8: *des->base.u8 = (uint8_t) inte; break;
		case I16: case U16: *des->base.u16 = (uint16_t) inte; break;
		case I32: case U32: *des->base.u32 = (uint32_t) inte; break;
		case I64: case U64: *des->base.u64 = inte; break;
		case F32: memcpy(des->base.p, &inte, 4); break;
		case F64: memcpy(des->base.p, &inte, 8); break;
	}
	return des;
}

nry_t* strcpytonry(nry_t* des, char* str){
	size_t i = 0;
	while(str[i] != '\0'){
		i++;
	}
	remakenry(des, i);
	memcpy(des->base.p, str, i);
	return des;
}

nry_t* strtonry(nry_t* des, char* str, int* start){
	int i = *start;
	size_t j = 0;
	while(str[i] != '"' && str[i] != '\0'){
		if(str[i] == '\\') i++;
		i++; j++;
	}
	remakenry(des, j);
//	printf("len of str %lu\n", j);
	j = 0;
	int n;
	for(n = *start; n < i; n++){
		if(str[n] == '\\'){
			switch (str[n+1]){
				case '0': des->base.chr[j] = '\0'; break;
				case 'a': des->base.chr[j] = '\a'; break;
				case 'b': des->base.chr[j] = '\b'; break;
				case 't': des->base.chr[j] = '\t'; break;
				case 'v': des->base.chr[j] = '\v'; break;
				case 'f': des->base.chr[j] = '\f'; break;
				case 'r': des->base.chr[j] = '\r'; break;
				case 'n': des->base.chr[j] = '\n'; break;
				case '\\': des->base.chr[j] = '\\'; break;
				case '\"': des->base.chr[j] = '\"'; break;
			}
			n++;
		} else des->base.chr[j] = str[n];
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

ptr_t inputtonrs(ptr_t des, char* input, int start, int* end, int type){
	uint64_t nr = inputtoint(input, start, end);
	switch(type){
		case Chr...U8: *des.u8 = (uint8_t) nr; break;
		case I16...U16: *des.u16 = (uint16_t) nr; break;
		case I32...U32: *des.u32 = (uint32_t) nr; break;
		case I64...U64: *des.u64 = (uint64_t) nr; break;
		case F32: *des.f32 = (float) (int64_t) nr; break;
		case F64: *des.f64 = (double) (int64_t) nr; break;
	}
	return des;
}

nry_t* inptonry(nry_t* des, char* input, int* start, int type){
	int i = *start;
	int tl = typelen[type];
	des->len = tl;
	while(IsNr(input + i) || IsSpace(input + i) || input[i] == '+' || input[i] == '\'')
		if(input[i++] == '+') des->len += tl;
	remakenry(des, des->len);

	int argnr = 0;
	i = *start;
	while(IsNr(input + i) || IsSpace(input + i) || input[i] == '+' || input[i] == '\''){
		if(input[i] == '+') argnr += tl;
		else if(input[i] == '\'') des->fst.u8 = des->base.u8 + argnr;
		else if(IsNr(input + i)) inputtonrs((ptr_t) (des->base.u8 + argnr), input, i, &i, type);
		i++;
	}
	*start = i;
	return des;
}

void aprintnry(nry_t* src, int type, bool endline){
	if(type == Chr){
		char saf = src->base.chr[src->len];
		src->base.chr[src->len] = '\0';
		printf("%s", src->fst.chr);
		src->base.chr[src->len] = saf;
		goto endoffunc;
	}
//	int inc = typeBylen(type);
	if(src->len == 0){ printf("'"); goto endoffunc;}
	for(size_t i = 0; i*typelen[type] < src->len; i += 1) {
		if(src->base.u8 + i == src->fst.u8) printf("'");
		switch(type){
			case I8: printf("%d + ", *(src->base.i8 + i)); break;
			case U8: printf("%u + ", *(src->base.u8 + i)); break;
			case I16: printf("%d + ", *(src->base.i16 + i)); break;
			case U16: printf("%u + ", *(src->base.u16 + i)); break;
			case I32: printf("%d + ", *(src->base.i32 + i)); break;
			case U32: printf("%u + ", *(src->base.u32 + i)); break;
			case I64: printf("%ld + ", *(src->base.i64 + i)); break;
			case U64: printf("%lu + ", *(src->base.u64 + i)); break;
			case F32: printf("%f + ", *(src->base.f32 + i)); break;
			case F64: printf("%lf + ", *(src->base.f64 + i)); break;
		};
	}
	printf("\b\b\b   \b\b\b");
	endoffunc:
	if(endline) printf("\n");
}

bool equalnry(nry_t* a, nry_t* b){
	uint64_t enda;
	uint64_t endb;
	memcpy(&enda, a->base.u8 + a->len, 8);
	memcpy(&endb, b->base.u8 + b->len, 8);
	memset(a->base.u8 + a->len, 0, 8);
	memset(b->base.u8 + b->len, 0, 8);
	bool same  = true;
	if(a->len != b->len) {same = false; goto end;}
	for(size_t i = 0; i < a->len; i+=8)
		if( *((ptr_t)(a->base.u8 + i)).u64 != *((ptr_t)(b->base.u8 + i)).u64){ same = false; goto end;}
	end:
	memcpy(a->base.u8 + a->len, &enda, 8);
	memcpy(b->base.u8 + b->len, &endb, 8);
	return same;
}

void printnrydebug(nry_t* src){
	printf("Debug print nry: %p --------\n", src);
	printf("base: %p, fst: %p, len: %lu\n", src->base.p, src->fst.p, src->len);
	printf("base + len: %p\n", src->base.u8 + src->len);
	printf("Actual offset: %lu\n", src->fst.p - src->base.p);
	printf("--------\n");
}

#endif

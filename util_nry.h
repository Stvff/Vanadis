#ifndef UTIL_NRY_H
#define UTIL_NRY_H
#include "util_common.h"

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

int typeBylen(int type){
	switch(type){
		case Chr...U8: return 1; break;
		case I16: case U16: return 2; break;
		case I32: case U32: case F32: return 4; break;
		case I64: case U64: case F64: return 8; break;
	}
	return 1;
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
	return 0;
}

int64_t sinteger(uint8_t* ptr, int type){
	switch(type){
		case Chr...U8: return i8 ptr; break;
		case I16: case U16: return i16 ptr; break;
		case I32: case U32: return i32 ptr; break;
		case I64: case U64: return i64 ptr; break;
		case F32: return (int64_t) f32 ptr; break;
		case F64: return (int64_t) f64 ptr; break;
	}
	return 0;
}

// ######################################################################################## nry stuff
typedef struct {
	uint8_t* base;
	uint64_t len;
	uint8_t* fst;
} nry_t;

nry_t* makenry(nry_t* des, uint64_t len){
	des->base = malloc(len + 8);
	if(des->base == NULL) printf("WHAT???? HOW???? WHAT NOW??? (makenry)\n");
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
	if(des->base == NULL) printf("WHAT???? HOW???? WHAT NOW??? (remakenry)\n");
	des->fst = des->base;
	memset(des->base + len, 0, 8);
	des->len = len;
	return des;
}

nry_t* makeimnry(nry_t* des, nry_t* src){
	des->base = malloc(src->len + 8);
	if(des->base == NULL) printf("WHAT???? HOW???? WHAT NOW??? (makeimnry)\n");
	des->len = src->len;
	memset(des->base + des->len, 0, 8);
	des->fst = des->base + (src->fst - src->base);
	des->base = memcpy(des->base, src->base, des->len);
	return des;
}

nry_t* copynry(nry_t* des, nry_t* src){
	if(des->base == src->base) return des;
	des->base = realloc(des->base, src->len + 8);
	if(des->base == NULL) printf("WHAT???? HOW???? WHAT NOW??? (copynry)\n");
	des->len = src->len;
	memset(des->base + des->len, 0, 8);
	des->fst = des->base + (src->fst - src->base);
	des->base = memcpy(des->base, src->base, des->len);
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
//	printf("len of str %lu\n", j);
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
		case Chr...U8: u8 des = (uint8_t) nr; break;
		case I16...U16: u16 des = (uint16_t) nr; break;
		case I32...U32: u32 des = (uint32_t) nr; break;
		case I64...U64: u64 des = (uint64_t) nr; break;
		case F32: f32 des = (float) (int64_t) nr; break;
		case F64: f64 des = (double) (int64_t) nr; break;
	}
	return des;
}

nry_t* inptonry(nry_t* des, char* input, int* start, int type){
	int i = *start;
	int tl = typeBylen(type);
	des->len = tl;
	while(IsNr(input + i) || IsSpace(input + i) || input[i] == '+' || input[i] == '\'')
		if(input[i++] == '+') des->len += tl;
	remakenry(des, des->len);

	int argnr = 0;
	i = *start;
	while(IsNr(input + i) || IsSpace(input + i) || input[i] == '+' || input[i] == '\''){
		if(input[i] == '+') argnr += tl;
		else if(input[i] == '\'') des->fst = des->base + argnr;
		else if(IsNr(input + i)) inputtonrs(des->base + argnr, input, i, &i, type);
		i++;
	}
	*start = i;
	return des;
}

void aprintnry(nry_t* src, int type, bool endline){
	if(type == Chr){
		char saf = src->base[src->len];
		src->base[src->len] = '\0';
		printf("%s", src->fst);
		src->base[src->len] = saf;
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
	uint64_t enda = u64 (a->base + a->len);
	uint64_t endb = u64 (b->base + b->len);
	u64 (a->base + a->len) = 0;
	u64 (b->base + b->len) = 0;
	bool same  = true;
	if(a->len != b->len) {same = false; goto end;}
	for(uint64_t i = 0; i < a->len; i+=8)
		if(u64 (a->base + i) != u64 (b->base + i)){ same = false; goto end;}
	end:
	u64 (b->base + b->len) = enda;
	u64 (b->base + b->len) = endb;
	return same;
}

void printnrydebug(nry_t* src){
	printf("Debug print nry: %p--------\n", src);
	printf("base: %p, fst: %p, len: %lu\n", src->base, src->fst, src->len);
	printf("base + len: %p\n", src->base + src->len);
	printf("Debug print nry over --------\n");
}

#endif

#ifndef VCO_H
#define VCO_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SN *(int8_t*)
#define UN *(uint8_t*)
#define SL *(int64_t*)
#define UL *(uint64_t*)

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

nry_t* inttonry(nry_t* des, uint64_t inte, int form){
	remakenry(des, 1 + 7*(form%2));
	switch(form){
		case 0: SN des->base = (int8_t) inte; break;
		case 1: SL des->base = (int64_t) inte; break;
		case 2: UN des->base = (uint8_t) inte; break;
		case 3: UL des->base = (uint64_t) inte; break;
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

uint64_t inputtoint(char* input, int start, int* end, bool considersign){
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
		if(entry == '-'){
			if(considersign) negative = !negative;
		} else if(entry != ' '){
			accum += (entry - 0x30)*pow;
			pow *= 10;
		}
	}
	if(negative) accum *= -1;
	return accum;
}

int format(char* input, int start){
	bool Long = false;
	bool Unsi = false;
	int i = start;
	char entry = input[i];
	while(!EndLine(&entry)){
		entry = input[++i];
//		printf("format, entry: %c\n", entry);
		if(entry == 'n' || entry == 'N') Long = false;
		if(entry == 's' || entry == 'S') Unsi = false;
		if(entry == 'l' || entry == 'L') Long = true;
		if(entry == 'u' || entry == 'U') Unsi = true;
	}
	int form = 0;
	if(Long) form += 1;
	if(Unsi) form += 2;
	return form;
}

void aprintnry(nry_t* src, int form, bool endline){
	int inc = 1 + 7*(form%2);
	if(src->len == 0){ printf("'"); goto endoffunc;}
	for(uint64_t i = 0; i < src->len; i += inc) {
		if(src->base + i == src->fst) printf("'");
		switch(form){
			case 0: printf("%d + ",  SN (src->base + i)); break;
			case 1: printf("%ld + ", SL (src->base + i)); break;
			case 2: printf("%d + ",  UN (src->base + i)); break;
			case 3: printf("%lu + ", UL (src->base + i)); break;
		};
	}
	printf("\b\b\b   \b\b\b");
	endoffunc:
	if(endline) printf("\n");
}

bool equalnry(nry_t* a, nry_t* b){
	if(a->len != b->len) return false;
	for(uint64_t i = 0; i < a->len; i++)
		if(a->base[i] != b->base[i]) return false;
	return true;
}

nry_t* rshiftnry(nry_t* des, uint64_t shift, int form){
	if(shift == 0) return des;
	uint64_t off = des->fst - des->base;
	if(form < 2 && (int64_t) shift < 0) goto shiftdown;

	des->base = realloc(des->base, des->len + shift + 8);
	memset(des->base + des->len, 0, shift + 8);
	des->len += shift;
	des->fst = des->base + off;
	return des;

	shiftdown:
	shift *= -1;
	if(shift >= des->len){ freenry(des); makenry(des, 0); return des;}
	des->len -= shift;
	des->base = realloc(des->base, des->len + 8);
	memset(des->base + des->len, 0, 8);
	des->fst = des->base + off%des->len;
	return des;
}

nry_t* shiftnry(nry_t* des, uint64_t shift, int form){
	if(shift == 0) return des;
	uint64_t off = des->fst - des->base;
	if(form < 2 && (int64_t) shift < 0) goto shiftdown;

	des->base = realloc(des->base, des->len + shift + 8);
	memmove(des->base + shift, des->base, des->len);
	memset(des->base, 0, shift);
	des->len += shift;
	memset(des->base + des->len, 0, 8);
	des->fst = des->base + off;
	return des;

	shiftdown:
	shift *= -1;
	if(shift >= des->len){ freenry(des); makenry(des, 0); return des;}
	des->len -= shift;
	memmove(des->base, des->base + des->len, shift);
	des->base = realloc(des->base, des->len + 8);
	memset(des->base + des->len, 0, 8);
	des->fst = des->base + off%des->len;
	return des;
}

nry_t* cutnry(nry_t* des, nry_t* src, uint64_t start, uint64_t end){
	uint64_t temp = end;
	if(start%src->len > end%src->len){ end = start; start = temp;}
	temp = end - start;
	printf("start: %lu, end: %lu, temp: %lu\n", start, end, temp);
	nry_t* ptr = des;
	if(des == src){
		nry_t part; makenry(&part, temp);
		ptr = &part;
	} else {
		remakenry(des, temp);
	}
	for(uint64_t i = 0; i < temp; i++)
		ptr->base[i] = src->base[(start + i) % src->len];
	if(des == src){
		copynry(des, ptr);
		freenry(ptr);
	}
	return des;
}

// ######################################################################################## machine functions
#define userInputLen 256
#define maxKeywordLen 16
#define argumentAmount 4

char* UserInput;

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

nry_t** stack;
nry_t** codex;
int64_t stackPtr;
int64_t codexPtr;
nry_t regs[regAmount];

time_t thetime;

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
	for(int i = 0; i < regAmount; i++){
		makenry(&regs[i], 8);
		memset(regs[i].base, 0, 8);
	}

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
	for(int i = 0; i < regAmount; i++) freenry(&regs[i]);
}

#endif

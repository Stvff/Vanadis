#ifndef UTIL_MEMWY_H
#define UTIL_MEMWY_H
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>

typedef size_t pos_t;
typedef struct {
	size_t avai;
	size_t used;
	pos_t pos;
	char* mem;
} memowy_t;

#define MEMOWYBUF 512
#define data(memwy) ((memwy)->mem + (memwy)->pos + 2)
#define lenp(memwy) ((memwy)->mem + (memwy)->pos)

uint16_t lenwy(char* lp){
	uint16_t len; memcpy(&len, lp, 2); return len;
}

size_t allocmemwy(memowy_t* mp, char* src, uint16_t len){
	mp->used += len + 4; // 2*sizeof(uint16_t)
	while (mp->used > mp->avai) {
		mp->avai += MEMOWYBUF;
		mp->mem = realloc(mp->mem, mp->avai); 
	}
	memcpy(mp->mem + mp->used - (len + 4), &len, 2);
	memcpy(mp->mem + mp->used - 2, &len, 2);
	memmove(mp->mem + mp->used - (len + 2), src, len);
	return mp->used - (len + 4);
}

size_t freememwy(memowy_t* mp){
	if(mp->used == 0) return 0;
	mp->used -= lenwy(mp->mem + mp->used - 2) + 4;
	return mp->used - lenwy(mp->mem + mp->used);
}

size_t reallocmemwy(memowy_t* mp, uint16_t len){
	uint16_t oldlen = lenwy(lenp(mp));
	size_t remregionlen = mp->used - mp->pos - oldlen - 4;
	mp->used += len - oldlen;
	while (mp->used > mp->avai) {
		mp->avai += MEMOWYBUF;
		mp->mem = realloc(mp->mem, mp->avai);
	}
	char* loc = lenp(mp);
	memmove(loc + len + 4, loc + oldlen + 4, remregionlen);
	memcpy(loc, &len, 2);
	memcpy(loc + len + 2, &len, 2);
	return len;
}

size_t appmemwy(memowy_t* mp, char* src, uint16_t len){
	uint16_t oldlen = lenwy(lenp(mp));	
	reallocmemwy(mp, oldlen + len);
	memmove(data(mp) + oldlen, src, len);
	return len;
}

int indexmemwy(memowy_t* mp, int steps){
	size_t len = 0;
	while(steps != 0){
		if(steps < 0){
			memcpy(&len, lenp(mp) - 2, 2);
			mp->pos -= len + 4;
			steps++;
		} else {
			memcpy(&len, lenp(mp), 2);
			if(mp->pos + len + 4 >= mp->used){break;}
			mp->pos += len + 4;
			steps--;
		}
	}
	return steps;
}

void printmemwy(memowy_t* mp){
	pos_t oldpos = mp->pos;
	mp->pos = 0;
	do {
		*(data(mp) + lenwy(lenp(mp))) = '\0';
		printf("%s\n", data(mp));
		memcpy(data(mp) + lenwy(lenp(mp)), lenp(mp), 2);
	} while(indexmemwy(mp, 1) == 0);
	mp->pos = oldpos;
}

pos_t findmemwy(memowy_t* mp, pos_t target){
	pos_t oldpos = mp->pos;
	mp->pos = 0;
	pos_t find = mp->avai;
	do {
		if(mp->pos == target) continue;
		if(lenwy(mp->mem + target) != lenwy(lenp(mp))) continue;
		for(size_t i = 0; i < lenwy(lenp(mp)); i++) if(*(data(mp) + i) != *(mp->mem + target + 2 + i)) goto haha;
//		printf("same: %s\n", data(mp));
		find = mp->pos;
		break;
		haha:;
	} while(indexmemwy(mp, 1) == 0);
	mp->pos = oldpos;
	return find;
}

#endif

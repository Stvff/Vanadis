#ifndef IOSTR_H
#define IOSTR_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

// ######################################################################################## custom string functions
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

char SkipSpaces(char* str, int len, int* i){
	str += *i;
	while((*str == ' ' || *str == '\t') && *i < len){
		str++;
		(*i)++;
	}
	return *str;
}

int keywordlook(char string[], int thewordlen, char source[][thewordlen], int* readhead){
	int item = 0;
	int j;
	bool isthis;
	while (source[item][0] != '\0'){
		isthis = true;
		j = 0;
//		printf("keyword %s?\n", source[item]);
		while (source[item][j] != '\0'){
			if(string[j + *readhead] != source[item][j]) isthis = false;
			j++;
		}
		if(isthis){
			*readhead += --j;
			return item;
		}
		item++;
	}
	return -1;
}

int labellook(char* string, char** source){
	int item = 0;
	int j;
	bool isthis;
	while (source[item][0] != '\0'){
		isthis = true;
		j = 0;
//		printf("label %s?\n", source[item]);
		while (source[item][j] != '\0' && string[j] != '\0'){
			if(string[j] != source[item][j]) isthis = false;
			j++;
		}
		if(isthis)
			return item;
		item++;
	}
	return -1;
}


int64_t psi(int64_t g,int64_t h,int64_t n){
	return (n % (int64_t)pow((float)g, (float)(h+1)))/(int64_t)pow((float)g,(float)h);
}

uint8_t printinttostr(char* string, uint8_t integ){
	char chrrr;
	bool itmaynotzero = true;
	for(int i = 0; i < 3; i++){
		chrrr = psi(10, 2 - i, integ) + 0x30;
		if(i == 2) itmaynotzero = false;
		if(chrrr == '0' && itmaynotzero) chrrr = ' ';
		else itmaynotzero = false;
		string[i] = chrrr;
	}
	return integ;
}

// ######################################################################################## custom file functions
typedef struct FILEstuff {
	uint64_t len;
	uint64_t pos;
	char* mfp;

//	struct FILEstuff* prevfile;
} file_t;

char* mfgets(char* string, int size, file_t* file){
	int64_t i = 0;
	if(file->pos == file->len) return NULL;
	do {
		string[i] = file->mfp[file->pos + i];
		i++;
	} while(file->mfp[file->pos + i - 1] != '\n' && i <= size-1 && file->pos + i <= file->len);

	string[i] = '\0';
	file->pos += i;
//	printf("-------------------------------- %s\n", string);
	return string;
}

char* mfgetsS(char* string, int size, file_t* file){
	uint64_t i = file->pos + size;
	char* retptr = string;
	if(i > file->len) i = file->len;
	if(file->pos == file->len) return NULL;
	do *string = file->mfp[file->pos++];
	while(*string != '\n' && *(string++) != ';' && file->pos < i);
	*string = '\0';
//	printf("-------------------------------- %s\n", string);
	return retptr;
}

file_t* mfopen(char* path, file_t* file){
	FILE* fp = fopen(path, "r");
	if(fp == NULL){
		printf("\aCould not open file '%s'.\n", path);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	file->len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	file->mfp = malloc(file->len);
	fread(file->mfp, file->len, 1, fp);
	file->pos = 0;

	fclose(fp);
	return file;
}

void mfclose(file_t* file){
	if(file->mfp == NULL) return;
	free(file->mfp); file->mfp = NULL;
	file->pos = 0;
	file->len = 0;
}

file_t* mfapp(file_t* file, char* src, uint64_t len){
	file->mfp = realloc(file->mfp, file->len + len);
	memcpy(file->mfp + file->len, src, len);
	file->len += len;
//	printf("%lx, len: %ld\n", (uint64_t) file->mfp, file->len);
	return file;
}

#endif

#ifndef VANADIS_H
#define VANADIS_H
#include "vco.h"

// ######################################################################################## instructions
char instructionString[][maxKeywordLen] = {
	"set", "dset", "iget", "iset",
	"inc", "dec", "add", "sub", "rsub", "mul", "div", "mod",
	"app", "rot", "shf", "rshf", "rev", "sel", "cut", "ins",
	"ptra", "ptrs", "ptr", "len", "fst",
	"push", "pop", "peek", "flip", "unf",
	"cmp", "equ", "Ce", "Cs", "Cg", "Cn",
	"ninput", "sinput",
	"fread", "fwrite", "flen",
	"rmr", "jmp", "run", "prun", "rjmp",
	"print", "nprint", "sprint",
	"\\", "\0end"
};

enum instructionEnum {
	set, dset, iget, iset,
	inc, dec, add, sub, rsub, mul, divi, modu,
	app, rot, shf, rshf, rev, sel, cut, ins,
	ptra, ptrs, ptr, len, fstF,
	push, pop, peek, flip, unf,
	cmp, equ, Ce, Cs, Cg, Cn,
	ninput, sinput,
	firead, fiwrite, flen,
	rmr, jmp, run, prun, rjmp,
	print, nprint, sprint,
	endprog
};

// ######################################################################################## custom string functions
int strlook(char string[], int thewordlen, char source[][thewordlen], int* readhead){
	int item = 0;
	int j;
	bool isthis;
	while (source[item][0] != '\0'){
		isthis = true;
		j = 0;
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

// ######################################################################################## custom file functions
#define labelSize 20

typedef struct PREVfil {
	int fileNr;
	uint64_t prevfile;
} prev_t;

typedef struct FILEstuff {
	uint64_t len;
	uint64_t pos;
	char* mfp;
	prev_t prev;
	uint64_t* labelposs;
	char (*labels)[labelSize];
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

file_t* mfopen(char* path, file_t* file){
	FILE* fp = fopen(path, "r");
	if(fp == NULL){
		printf("\aCould not read file '%s'.\n", path);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	file->len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	file->mfp = malloc(file->len);
	fread(file->mfp, file->len, 1, fp);
	file->pos = 0;

	fclose(fp);

	file->labels = NULL;
	file->labelposs = NULL;
	int labelAm = 0;
	char buf[userInputLen];
	while(mfgets(buf, labelSize, file) != NULL){
		int i = 0;
		int j = 0;
		bool look = false;
		while(buf[i] != '\0' && i < userInputLen){
			if(buf[i] != ':' && EndLine(&buf[i])) break;
//			printf("%c, %d, %d\n", buf[i], i, look);
			if(look) {
				if(IsSpace(&buf[i])) break;
				if(j+2 >= labelSize) break;
				file->labels[labelAm - 1][j] = buf[i];
				file->labels[labelAm - 1][++j] = '\0';
//				printf("%c", buf[i]);
			} else if(buf[i] != ':' && !IsSpace(&buf[i])) break;
			if(buf[i] == ':'){
				look = true;
				labelAm++;
				file->labels = realloc(file->labels, sizeof(char[labelAm][labelSize]));
				file->labelposs = realloc(file->labelposs, sizeof(uint64_t[labelAm]));
				file->labelposs[labelAm - 1] = file->pos;
//				printf("\n");
			}
			i++;
		}
//		printf("------------------------------------\n");
	}
	file->labels = realloc(file->labels, sizeof(char[labelAm + 1][labelSize]));
	file->labels[labelAm][0] = '\0';

	file->pos = 0;
	return file;
}

void mfclose(file_t* file){
	free(file->mfp);
	free(file->labelposs);
	free(file->labels);
	file->mfp = NULL;
	file->labelposs = NULL;
	file->labels = NULL;
	file->pos = 0;
	file->len = 0;
}

// ######################################################################################## machine functions
// ######################################################################################## machine functions
// ######################################################################################## machine functions

#endif

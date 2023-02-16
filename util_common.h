#ifndef UTIL_COMMON_H
#define UTIL_COMMON_H
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

bool EndLine(char entry){
	return entry == '\n' || entry == '\r' || entry == '\0' || entry == ';' || entry == ':' || entry == '#';
}

bool IsNr(char entry){
	return (entry >= '0' && entry <= '9') || entry == '-';
}

bool IsAlph(char entry){
	return (entry >= 'a' && entry <= 'z') || (entry >= 'A' && entry <= 'Z');
}

bool IsPage(char entry){
	return (entry >= '0' && entry <= '9') || entry == '-' || entry == '+'
		|| entry == '\'' || entry == '.' || entry == '"';
}

bool IsSpace(char* entry){
	return *entry == ' ' || *entry == '\t';
}

size_t delimstrlen(char* str, char chra){
	size_t i = 0;
	while(str[i] != '\0' && str[i] != chra) i++;
	return i;
}

char SkipSpaces(char* str, size_t len, size_t* i){
	str += *i;
	while(*i < len && (*str == ' ' || *str == '\t')){
		str++;
		(*i)++;
	}
	return *str;
}

char SkipString(char* str, int len, int* i){
	str += *i;
	char esc = 1;
	printf("len: %d\n", len);
	while(*i < len && (*str != '"' || esc)){
		esc = *str != '\\' && !esc;
		str++;
		(*i)++;
		printf("i: %d, chr: %c\n", *i, *str);
	}
	return *str;
}

// ######################################################################################## custom file functions
typedef struct FILEstuff {
	size_t len;
	size_t pos;
	char* mfp;
//	struct FILEstuff* prevfile;
	size_t strlen;
	size_t strbuf;
	size_t strpos;
	char* str;

	char* name;
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

char* mfgetsSu(file_t* file){
	if(file->len == file->pos) return NULL;

	size_t len = file->pos; {
		bool inside = false;
		bool escaped = false;
		while(1){
			if(len >= file->len || file->mfp[len] == '\n' || file->mfp[len] == '\r') break;
			escaped = inside && !escaped && file->mfp[len] == '\\';
			inside = (!escaped && file->mfp[len] == '"') ^ !inside;
			if(!inside && (file->mfp[len-1] == '#' || file->mfp[len-1] == ';')) break;
			len++;
		} len++;
	}

	//TODOFIXME: handle updating file->pos and pure comments

	len -= file->pos;
	if(len > file->strbuf){
		file->str = realloc(file->str, len + 1);
		file->strbuf = len + 1;
	}
	memcpy(file->str, file->mfp + file->pos, len);
	file->str[len] = '\0';
	file->strlen = len;
	file->strpos = 0;
	return file->str;
}

file_t* mfopen(char* path, file_t* file){
	FILE* fp = fopen(path, "r");
	if(fp == NULL){
		fprintf(stderr, "\aVanadis: \033[91mCould not open file '%s'.\033[0m\n", path);
		return NULL;
	}
	file->name = malloc(strlen(path) + 1);
	strcpy(file->name, path);

	fseek(fp, 0, SEEK_END);
	file->len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	file->mfp = malloc(file->len);
	fread(file->mfp, file->len, 1, fp);
	file->pos = 0;

	fclose(fp);

	file->strlen = 0;
	file->strbuf = 0;
	file->strpos = 0;
	file->str = NULL;
	return file;
}

void mfclose(file_t* file){
	free(file->mfp);
	file->mfp = NULL;
	file->pos = 0;
	file->len = 0;

	free(file->str);
	file->str = NULL;
	file->strlen = 0;
	file->strbuf = 0;
	file->strpos = 0;

	free(file->name);
}

file_t* mfapp(file_t* file, char* src, size_t len){
	file->mfp = realloc(file->mfp, file->len + len);
	memcpy(file->mfp + file->len, src, len);
	file->len += len;
//	printf("%lx, len: %ld\n", (uint64_t) file->mfp, file->len);
	return file;
}

file_t* mfins(file_t* des, size_t pos, char* src, size_t len){
	des->mfp = realloc(des->mfp, des->len + len);
	memmove(des->mfp + pos + len, des->mfp + pos, des->len - pos);
	memcpy(des->mfp + pos, src, len);
	des->len += len;
	return des;
}

#endif

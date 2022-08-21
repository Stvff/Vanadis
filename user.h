#ifndef USER_GHHGHGHG
#define USER_GHHGHGHG
#include "iostr.h"
#include "vic.h"

void error(char* errormessage, int readhead, file_t* file){
	int newlines = 0;
	for(uint64_t i = 0; i < file->pos; i++)
		if(file->mfp[i] == '\n') newlines++;
	printf("%s at the %dth char:\n", errormessage, readhead);
	int skip = printf(" %d|", newlines);
	printf("%s\n", UserInput);
	for(int i = 0; i < readhead + skip - 1; i++)
		printf(" ");
	printf("<^>\n");
}

bool checkkinds(signed char ins, char* kinds, file_t* file){
	bool good = true;
//	printf("checking kinds: %s %s? %s\n", instructionString[ins], kinds, instructionKinds[ins]);
	for(signed char i = 0; (i < argumentAmount) && good; i++)
		good = kinds[i] == instructionKinds[ins][i];
	if(good) return good;
	int newlines = 0;
	for(uint64_t i = 0; i < file->pos; i++)
		if(file->mfp[i] == '\n') newlines++;
	printf("\aThe given arguments on the following line were not of the correct kind:\n");
	printf(" %d|%s\n\n", newlines, UserInput);
	printf("The instruction '%s' expects:\n\t%s ", instructionString[ins], instructionString[ins]);
	for(signed char i = 0; i < argumentAmount && instructionKinds[ins][i] != '_'; i++)
		printf("<%s>, ", instructionKinds[ins][i]=='p'?"page":"datum");
	printf("\b\b;\nbut was given:\n\t%s ", instructionString[ins]);
	for(signed char i = 0; i < argumentAmount && kinds[i] != '_'; i++)
		printf("<%s>, ", kinds[i]=='p'?"page":"datum");
	printf("\b\b;\n");
	return false;
}

typedef struct {
	uint32_t bindam;
	char** binds;
	char** resos;
} bind_t;

bind_t* bindo(file_t* file, char* input, int* readhead, bind_t* binds){
	SkipSpaces(UserInput, userInputLen, readhead);
	if(input[*readhead] < 'A' || input[*readhead] > 'Z'){
		error("\aBindings must start with a capital letter.\n", *readhead, file);
		return NULL;
	}
	int i = *readhead;
	while(IsAlph(input + i) || IsNr(input + i) || input[i] == '_')
		i++;
	binds->bindam++;
	binds->binds = realloc(binds->binds, sizeof(char*[binds->bindam]));
	binds->binds[binds->bindam-1] = malloc(i - *readhead + 1);
	memcpy(binds->binds[binds->bindam-1], input + *readhead, i - *readhead);
	binds->binds[binds->bindam-1][i - *readhead] = '\0';

	
	return binds;
}

bind_t* insertbind(file_t* file, int* readhead, bind_t* binds){
	
}

typedef struct {
	uint32_t labelam;
	uint64_t* labelpos_s;
	char** definedlabels;
	uint32_t jumpam;
	uint64_t* jumppos_s;
	char** requiredlabels;
} lbl_t;

lbl_t* savelabel(file_t* file, char* input, int* readhead, lbl_t* labels, file_t* src){
//	printf("saving label\n");
	labels->labelam++;
	labels->labelpos_s = realloc(labels->labelpos_s, sizeof(uint64_t[labels->labelam]));
	labels->labelpos_s[labels->labelam-1] = file->len;
//	printf("labelpos 0x%lx nr %d\n", labels->labelpos_s[labels->labelam-1], labels->labelam-1);
	labels->definedlabels = realloc(labels->definedlabels, sizeof(char*[labels->labelam]));
	int i = *readhead;
//	printf("length?\n");
	while(IsAlph(input + i) || IsNr(input + i) || input[i] == '_')
		i++;
	if(i - *readhead == 0){
		labels->definedlabels[labels->labelam-1] = malloc(1);
		error("\aLabel can not be 0 characters long, allowed characters are: a-z, A-Z, 0-9, _ and -.\n", i, src);
		return NULL;
	}
//	printf("copy it\n");
	labels->definedlabels[labels->labelam-1] = malloc(i - *readhead + 2);
	memcpy(labels->definedlabels[labels->labelam-1], input + *readhead, i - *readhead + 1);
	labels->definedlabels[labels->labelam-1][i - *readhead] = ' ';
	labels->definedlabels[labels->labelam-1][i - *readhead + 1] = '\0';
//	printf("done\n");
	return labels;
}

lbl_t* savejmp(file_t* file, char* input, int* readhead, lbl_t* labels, file_t* src){
//	printf("saving jump\n");
	labels->jumpam++;
	labels->jumppos_s = realloc(labels->jumppos_s, sizeof(char*[labels->jumpam+1]));
	labels->jumppos_s[labels->jumpam-1] = file->len;
//	printf("jumppos 0x%lx nr %d\n", labels->jumppos_s[labels->jumpam-1], labels->jumpam-1);
	labels->requiredlabels = realloc(labels->requiredlabels, sizeof(char*[labels->jumpam]));
	int i = *readhead;
//	printf("length?\n");
	while(IsAlph(input + i) || IsNr(input + i) || input[i] == '_')
		i++;
	if(i - *readhead == 0){
		labels->requiredlabels[labels->jumpam-1] = malloc(1);
		error("\aLabel can not be 0 characters long, allowed characters are: a-zA-Z0-9_-\n", i, src);
		return NULL;
	}
//	printf("copy it\n");
	labels->requiredlabels[labels->jumpam-1] = malloc(i - *readhead + 2);
	memcpy(labels->requiredlabels[labels->jumpam-1], input + *readhead, i - *readhead + 1);	
	labels->requiredlabels[labels->jumpam-1][i - *readhead] = ' ';
	labels->requiredlabels[labels->jumpam-1][i - *readhead + 1] = '\0';
//	printf("done\n");
	return labels;
}

int solvelabels(file_t* file, lbl_t* labels){
	int labelnr = -1;

	labels->definedlabels = realloc(labels->definedlabels, sizeof(uint64_t[labels->labelam + 1]));
	labels->definedlabels[labels->labelam] = malloc(1);
	labels->definedlabels[labels->labelam][0] = '\0';

	for(uint32_t i = 0; i < labels->jumpam; i++){
//		printf("Looking for label '%s'\n", labels->requiredlabels[i]);
//		dummy = 0;
		labelnr = labellook(labels->requiredlabels[i], labels->definedlabels);
		if(labelnr == -1){
			printf("\aLabel '%s' does not exist.\n", labels->requiredlabels[i]);
			free(labels->definedlabels[labels->labelam]);
			labels->definedlabels = realloc(labels->definedlabels, sizeof(char*[labels->labelam]));
			return -1;
		}
//		printf("hewewowo %d, 0x%lx\n", i, labels->labelpos_s[labelnr]);
		u64 (file->mfp + labels->jumppos_s[i]) = labels->labelpos_s[labelnr];
	}

	free(labels->definedlabels[labels->labelam]);
	labels->definedlabels = realloc(labels->definedlabels, sizeof(char*[labels->labelam]));
	return 0;
}

void freelabels(lbl_t* labels){
	if(labels->labelam != 0){
//		printf("freeing labels\n");
		free(labels->labelpos_s);
		for(uint32_t i = labels->labelam - 1; i != (uint32_t)-1; i--){
//			printf("%d\n", i);
			free(labels->definedlabels[i]);
		}
		free(labels->definedlabels);
	}
	if(labels->jumpam != 0){
//		printf("freeing jumps\n");
		free(labels->jumppos_s);
		for(uint32_t i = labels->jumpam - 1; i != (uint32_t)-1; i--){
//			printf("%d\n", i);
			free(labels->requiredlabels[i]);
		}
		free(labels->requiredlabels);
	}
//	printf("no error with that\n");
}

#endif

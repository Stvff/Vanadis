#ifndef USER_GHHGHGHG
#define USER_GHHGHGHG
#include "iostr.h"
#include "vic.h"

void error(char* errormessage, int readhead, file_t* file){
	int newlines = 0;
	for(uint64_t i = 0; i < file->pos; i++)
		if(file->mfp[i] == '\n') newlines++;
	printf("%s at the %dth char:\n", errormessage, readhead + 1);
	int skip = printf(" %d|", newlines);
	printf("%s\n", UserInput);
	for(int i = 0; i < readhead + skip - 1; i++)
		printf(" ");
	printf("<^>\n");
}

char* templkindsw(char thing[4][14], char inp){
	switch(inp){
		case 'd': return thing[0]; break;
		case 'p': return thing[1]; break;
		case 'D': return thing[2]; break;
		case 'P': return thing[3]; break;
	}
	return UserInput;
}

char* inskindsw(char thing[4][16], char inp, char goal){
	switch(inp){
		case 'd': if(goal == 'D') return thing[2]; else return thing[0]; break;
		case 'p': if(goal == 'P') return thing[3]; else return thing[1]; break;
		case 'D': if(goal == 'D') return thing[4]; else return thing[0]; break;
		case 'P': if(goal == 'P') return thing[5]; else return thing[1]; break;
	}
	return UserInput;
}

bool checkkinds(signed char ins, char* kinds, file_t* file){
	bool good = true;
//	printf("checking kinds: %s %s? %s\n", instructionString[ins], kinds, instructionKinds[ins]);
	for(signed char i = 0; (i < argumentAmount) && good; i++)
		good = (kinds[i] == instructionKinds[ins][i]) || (kinds[i] + 0x20 == instructionKinds[ins][i]);
	if(good) return good;
	int newlines = 0;
	for(uint64_t i = 0; i < file->pos; i++)
		if(file->mfp[i] == '\n') newlines++;
	printf("\aThe given arguments on the following line were not of the correct kind:\n");
	printf(" %d|%s\n\n", newlines, UserInput);

	printf("The instruction '%s' expects:\n\t%s ", instructionString[ins], instructionString[ins]);
	char template[4][14] = {"datum", "page", "mutable datum", "mutable page"};
	for(signed char i = 0; i < argumentAmount && instructionKinds[ins][i] != '_'; i++)
		printf("(%s), ", templkindsw(template, instructionKinds[ins][i]));

	char results[6][16] = {"datum", "page", "immutable datum", "immutable page", "mutable datum", "mutable page"};
	printf("\b\b;\nbut was given:\n\t%s ", instructionString[ins]);
	for(signed char i = 0; i < argumentAmount && kinds[i] != '_'; i++)
		printf("(%s), ", inskindsw(results, kinds[i], instructionKinds[ins][i]));

	printf("\b\b;\n");
	return false;
}

//##################################################################################### memowy

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
	return mp->used - lenwy(mp->mem + mp->used - 2);
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
	memcpy(loc + len, &len, 2);	
	return len;
}

size_t appmemwy(memowy_t* mp, char* src, uint16_t len){
	uint16_t oldlen = lenwy(lenp(mp));	
	reallocmemwy(mp, oldlen + len);
	memmove(data(mp) + oldlen, src, len);
	memcpy(data(mp) + lenwy(lenp(mp)), lenp(mp), 2);
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

size_t delimstrlen(char* str, char chra){
	size_t i = 0;
	while(str[i] != '\0' && str[i] != chra) i++;
	return i;
}

//##################################################################################### binds
typedef struct {
	uint32_t bindam;
	char** binds;
	char** resos;
} bind_t;

bind_t* letbind(file_t* file, int i, bind_t* binds);

bind_t* enumbind(file_t* file, int i, bind_t* binds){
	/* getting the bindbase */
	int bbase = delimstrlen(UserInput, '=');
	if(UserInput[bbase] != '='){
		error("\aTo assign a binding, use '='", bbase, file);
		return NULL;
	}
	bbase++; SkipSpaces(UserInput, userInputLen, &bbase);
	int bbaselen = strlen(UserInput + bbase);
	if(bbaselen == 0){
		error("\aNo binding", bbase + bbaselen, file);
		return NULL;
	}
	char whichref[3] = {'t', UserInput[bbase + bbaselen - 1] == '<'?'>':']', '\0'};
	/* saving UserInput and the bindbase */
	memowy_t memowy = {MEMOWYBUF, 0, 0, malloc(MEMOWYBUF)};
	allocmemwy(&memowy, UserInput + bbase, bbaselen);
	allocmemwy(&memowy, UserInput, userInputLen);
	/* splitting on spaces and saving them */
	int bindlen;
	int bindam = 0;
	SkipSpaces(UserInput, userInputLen, &i);
	do{
		if(UserInput[i] < 'A' || UserInput[i] > 'Z'){
			error("\aBindings must start with a capital letter.\n", i, file);
			free(memowy.mem);
			return NULL;
		}
		bindlen = delimstrlen(UserInput + i, ' ');
		allocmemwy(&memowy, UserInput + i, bindlen);
		i += bindlen;
		SkipSpaces(UserInput, userInputLen, &i);
		bindam++;
	} while(UserInput[i] != '=');
	/* making the bindings themselves */
	indexmemwy(&memowy, 2);
	pos_t remem;
	short bindcn = 0;
	do {
		remem = memowy.pos;
		memowy.pos = allocmemwy(&memowy, data(&memowy), lenwy(lenp(&memowy)));
		/* appending the ` = (expr)` */
		appmemwy(&memowy, " = ", 3);
		appmemwy(&memowy, memowy.mem + 2, lenwy(memowy.mem));
		appmemwy(&memowy, "   ", 3);
		printinttostr(data(&memowy) + lenwy(lenp(&memowy)) - 3, bindcn);
		appmemwy(&memowy, whichref, 3);
		/* passing it to letbind */
		memcpy(UserInput, data(&memowy), lenwy(lenp(&memowy)));
		letbind(file, 0, binds);
		/* recycle */
//		printmemwy(&memowy);
		freememwy(&memowy);
		memowy.pos = remem;
		indexmemwy(&memowy, 1);
		bindcn++;
	} while(bindcn < bindam);

	free(memowy.mem);
	return binds;
}

bind_t* letbind(file_t* file, int i, bind_t* binds){
	SkipSpaces(UserInput, userInputLen, &i);
	if(UserInput[i] < 'A' || UserInput[i] > 'Z'){
		error("\aBindings must start with a capital letter.\n", i, file);
		return NULL;
	}
	int begin = i;
	while(IsAlph(UserInput + i) || IsNr(UserInput + i) || UserInput[i] == '_')
		i++;
	binds->bindam++;
	binds->binds = realloc(binds->binds, sizeof(char*[binds->bindam + 1]));
	binds->binds[binds->bindam-1] = realloc(binds->binds[binds->bindam-1], i - begin + 1);
	binds->binds[binds->bindam] = malloc(1);

	memcpy(binds->binds[binds->bindam-1], UserInput + begin, i - begin);
	binds->binds[binds->bindam-1][i - begin] = '\0';
	binds->binds[binds->bindam][0] = '\0';

	binds->resos = realloc(binds->resos, sizeof(char*[binds->bindam + 1])); // allocating this one before any errors can take place
	free(binds->resos[binds->bindam-1]);
	binds->resos[binds->bindam-1] = NULL;
	binds->resos[binds->bindam] = malloc(1);
	binds->resos[binds->bindam][0] = '\0';

	while(UserInput[i] != '=' && !EndLine(UserInput + i)) i++;
	if(UserInput[i] != '='){
		error("\aTo assign a binding, use '='", i, file);
		return NULL;
	}
	i++;
	SkipSpaces(UserInput, userInputLen, &i);
	if(EndLine(UserInput + i)){
		error("\aNo binding", i, file);
		return NULL;
	}
	begin = i;
	while(!EndLine(UserInput + i)) i++;
	binds->resos[binds->bindam-1] = malloc(i - begin + 1);
	memcpy(binds->resos[binds->bindam-1], UserInput + begin, i - begin);
	binds->resos[binds->bindam-1][i - begin] = '\0';

	i = begin;
	if(UserInput[i] >= 'A' && UserInput[i] <= 'Z')
		if(labellook(UserInput + i, binds->binds) == -1){
			error("\aUndefined binding", i, file);
			return NULL;
	}
	return binds;
}

bind_t* insertbind(file_t* file, int* readhead, bind_t* binds){
	int i = *readhead;
	int bi = labellook(UserInput + i, binds->binds);
	if(bi == -1){
		error("\aUndefined binding", i, file);
		return NULL;
	}
	int blen = strlen(binds->binds[bi]);
	int rlen = strlen(binds->resos[bi]);
	userInputLen = STANDARDuserInputLen + rlen - blen;
	if(userInputLen > STANDARDuserInputLen)
		UserInput = realloc(UserInput, userInputLen);
/*	printf("start %p, end %p, src %p, des %p, wlen %d, endes %p, endsrc %p\nnewlen %d, srcoffset %d, desoffset %d\n",
			UserInput, UserInput + userInputLen, UserInput + *readhead + rlen, UserInput + *readhead + blen,
			STANDARDuserInputLen - *readhead - blen - 1, UserInput + *readhead + rlen + STANDARDuserInputLen - *readhead - blen - 1,
			UserInput + *readhead + blen + STANDARDuserInputLen - *readhead - blen - 1,
			userInputLen, *readhead + rlen, *readhead + blen);
*/	memmove(UserInput + *readhead + rlen,
			UserInput + *readhead + blen,
			STANDARDuserInputLen - *readhead - blen - 1);
	memcpy(UserInput + *readhead, binds->resos[bi], rlen);
	(*readhead)--;
	return binds;
}
//##################################################################################### labels

void freebinds(bind_t* binds){
	for(unsigned int i = 0; i < binds->bindam + 1; i++){
		free(binds->binds[i]);
		free(binds->resos[i]);
	}
	free(binds->binds);
	free(binds->resos);
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

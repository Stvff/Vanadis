#ifndef VA_COMPILE_H
#define VA_COMPILE_H
#include "util_common.h"
#include "util_memwy.h"
#include "util_nry.h"
#include "va_vic.h"
#include "va_run.h"

bool debugCompile = false;

// ######################################################################################## stringlook
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

// ######################################################################################## errors and such
void error(char* errormessage, int readhead, file_t* file){
	int newlines = 0;
	for(uint64_t i = 0; i < file->pos; i++)
		if(file->mfp[i] == '\n') newlines++;
	fprintf(stderr, "%s at the %dth char:\n", errormessage, readhead + 1);
	int skip = fprintf(stderr, " %d|", newlines);
	fprintf(stderr, "%s\n", UserInput);
	for(int i = 0; i < readhead + skip - 1; i++)
		if(i >= skip && IsSpace(UserInput + i - skip))
			fprintf(stderr, "%c", UserInput[i - skip]);
		else fprintf(stderr, " ");
	fprintf(stderr, "<^>\n");
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
	fprintf(stderr, "\aThe given arguments on the following line were not of the correct kind:\n");
	fprintf(stderr, " %d|%s\n\n", newlines, UserInput);

	fprintf(stderr, "The instruction '%s' expects:\n\t%s ", instructionString[ins], instructionString[ins]);
	char template[4][14] = {"datum", "page", "mutable datum", "mutable page"};
	for(signed char i = 0; i < argumentAmount && instructionKinds[ins][i] != '_'; i++)
		fprintf(stderr, "(%s), ", templkindsw(template, instructionKinds[ins][i]));

	char results[6][16] = {"datum", "page", "immutable datum", "immutable page", "mutable datum", "mutable page"};
	fprintf(stderr, "\b\b;\nbut was given:\n\t%s ", instructionString[ins]);
	signed char argnr;
	for(argnr = 0; argnr < argumentAmount && kinds[argnr] != '_'; argnr++)
		fprintf(stderr, "(%s), ", inskindsw(results, kinds[argnr], instructionKinds[ins][argnr]));
	if(argnr != 0)
		fprintf(stderr, "\b\b;\n");
	else fprintf(stderr, ";\n");
	return false;
}

//##################################################################################### includes
void* includef(file_t* desfile, int i, memowy_t* includes){
	/* allocating and checking if file is already included */
	SkipSpaces(UserInput, userInputLen, &i);
	int filenamelen = 0;
	while(!EndLine(UserInput + i + filenamelen) && !IsSpace(UserInput + i + filenamelen))
		filenamelen++;
	UserInput[i + filenamelen] = '\0';
	pos_t newinclude = allocmemwy(includes, UserInput + i, filenamelen + 1);
	if(findmemwy(includes, newinclude) != includes->avai){
//		printf("already included\n");
		freememwy(includes);
		return desfile;
	}
	/* opening and inserting file */
	file_t srcfile = {0, 0, NULL};
	quicmfptr = mfopen(includes->mem + newinclude + 2, &srcfile);
	if(quicmfptr == NULL){
		error("", i, desfile);
		return NULL;
	}
	srcfile = *quicmfptr;
	mfins(desfile, desfile->pos, srcfile.mfp, srcfile.len);
	mfclose(&srcfile);
	return desfile;
}

//##################################################################################### binds
typedef struct {
	uint32_t bindam;
	char** binds;
	char** resos;
} bind_t;

bind_t* letbind(file_t* file, int i, bind_t* binds);

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

bind_t* enumbind(file_t* file, int i, bind_t* binds){
	/* getting the bindbase */
	int bbase = delimstrlen(UserInput, '=');
	if(UserInput[bbase] != '='){
		error("\aTo assign a binding, use '='", bbase, file);
		return NULL;
	}
	bbase++; SkipSpaces(UserInput, userInputLen, &bbase);
	int bbaselen = delimstrlen(UserInput + bbase, ',');
	if(bbaselen == 0){
		error("\aNo binding", bbase + bbaselen, file);
		return NULL;
	}
	char whichref[2] = {UserInput[bbase + bbaselen - 1] == '<'?'>':']', '\0'};
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
	/* checking the specific type of the enum */
	int enumtype = globalType;
	if(UserInput[bbase + bbaselen] == ','){
		i = bbase + bbaselen;
		i++; SkipSpaces(UserInput, userInputLen, &i);
		enumtype = keywordlook(UserInput, 4, typeString, &i);
		if(enumtype == -1){
			error("\aInvalid type", i, file);
			free(memowy.mem);
			return NULL;
		}
	}
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
		printinttostr(data(&memowy) + lenwy(lenp(&memowy)) - 3, bindcn*typeBylen(enumtype));
		appmemwy(&memowy, whichref, 2);
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

void freebinds(bind_t* binds){
	for(unsigned int i = 0; i < binds->bindam + 1; i++){
		free(binds->binds[i]);
		free(binds->resos[i]);
	}
	free(binds->binds);
	free(binds->resos);
}

//##################################################################################### labels

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
	if(debugCompile) printf("Label '%s' is at 0x%lx\n", labels->definedlabels[labels->labelam-1], labels->labelpos_s[labels->labelam-1]);
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
			fprintf(stderr, "\aLabel '%s' does not exist.\n", labels->requiredlabels[i]);
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

char* arrapp(char* des, uint64_t deslen, char* src, uint64_t srclen){
	des = realloc(des, deslen + srclen);
	memcpy(des + deslen, src, srclen);
	return des;
}

//##################################################################################### building, compiling

char* buildargs(int* readhead, file_t* sourcefile, bind_t* bindings, char ins){
//	printf("buildargs\n");
	char* section = malloc(2);
	u16 section = 2;
	nry_t dnry; makenry(&dnry, 8);
	unsigned char commaam = 0;
	unsigned char prev = opNoop;
	char kinds[2] = {'_', '_'};
	char argkinds[argumentAmount + 1] = "____\0";
	SkipSpaces(UserInput, userInputLen, readhead);
	if(EndLine(UserInput + *readhead)) goto end;
	do{
		switch(UserInput[*readhead]){
			case '$':
//				printf("op is $ %x\n", opStackref);
				dummy = opStackref;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				if(kinds[0]!='d' && kinds[0]!='D'){
					error("\aOperator '$' expects a datum, not a page or nothing.\n", *readhead, sourcefile);
					goto endonerror;
				}
				kinds[0] = 'P';
				prev = opStackref;
				break;
			case '@':
//				printf("op is @ %x\n", opStackref);
				dummy = opStackrevref;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				if(kinds[0]!='d' && kinds[0]!='D'){
					error("\aOperator '@' expects a datum, not a page or nothing.\n", *readhead, sourcefile);
					goto endonerror;
				}
				kinds[0] = 'P';
				prev = opStackrevref;
				break;
			case '!':
//				printf("op is ! %x\n", opImm);
				if(prev == opStackref || prev == opStackrevref){
					u8 (section - 1 + u16 section) += 2;
					kinds[0] = 'D';
					prev += 2;
					break;
				}
				dummy = opImm;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				if(kinds[0]!='p' && kinds[0]!='P'){
					error("\aOperator '!' expects a page, not a datum or nothing.\n", *readhead, sourcefile);
					goto endonerror;
				}
				kinds[0] = kinds[0]=='P'?'D':'d';
				prev = opImm;
				break;
			case '^':
//				printf("op is ^ %x\n", opMakenry);
				dummy = opMakenry;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				if(kinds[0]!='d' && kinds[0]!='D'){
					error("\aOperator '^' expects a datum, not a page or nothing.\n", *readhead, sourcefile);
					goto endonerror;
				}
				kinds[0] = 'p';
				prev = opMakenry;
				break;
			case ']':
//				printf("op is ] %x\n", opEntry);
				dummy = opEntry;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				if(!(kinds[0]=='d' || kinds[1]=='p' || kinds[0]=='D' || kinds[1]=='P')){
					error("\aOperator ']' expects a page and a datum (in that order).\n", *readhead, sourcefile);
					goto endonerror;
				}
				kinds[0] = kinds[1]=='P'?'D':'d';
				prev = opEntry;
				break;
			case '>':
//				printf("op is > %x\n", opRef);
				dummy = opRef;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				if(!(kinds[0]=='d' || kinds[1]=='p' || kinds[0]=='D' || kinds[1]=='P')){
					error("\aOperator '>' expects a page and a datum (in that order).\n", *readhead, sourcefile);
					goto endonerror;
				}
				kinds[0] = kinds[1]=='P'?'D':'d';
				prev = opRef;
				break;
			case '*':
//				printf("op is *\n");
				if(prev == opEntry || prev == opRef){
					if(kinds[1]!='P'){
						error("\aThis operator expects its first argument to be a mutable page.\n", *readhead, sourcefile);
						goto endonerror;
					}
					u8 (section - 1 + u16 section) += 2;
					prev += 2;
				} break;
			case 'l':
//				printf("op is l %x\n", opLength);
				dummy = opLength;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				if(kinds[0]!='p' && kinds[0]!='P'){
					error("\aOperator 'l' expects a page, not a datum or nothing.\n", *readhead, sourcefile);
					goto endonerror;
				}
				kinds[0] = 'd';
				prev = opLength;
				break;
			case 'o':
//				printf("op is o %x\n", opLength);
				dummy = opOffset;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				if(kinds[0]!='p' && kinds[0]!='P'){
					error("\aOperator 'o' expects a page, not a datum or nothing.\n", *readhead, sourcefile);
					goto endonerror;
				}
				kinds[0] = 'd';
				prev = opOffset;
				break;
			case 't':
//				printf("op is t %x\n", opSizeof);
				dummy = opSizeof;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				if(kinds[0]!='d' && kinds[0]!='D'){
					error("\aOperator 't' expects a datum, not a page or nothing.\n", *readhead, sourcefile);
					goto endonerror;
				}
				kinds[0] = 'd';
				prev = opSizeof;
				break;
			case '~':
//				printf("op is ~ %x\n", opLength);
				dummy = opSwap;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				if(kinds[0] == '_' || kinds[1] == '_'){
					error("\aNot enough elements to swap.\n", *readhead, sourcefile);
					goto endonerror;
				}
				dummy = kinds[0]; kinds[0] = kinds[1]; kinds[1] = dummy;
				prev = opSwap;
				break;
			case ',':
//				printf("kind is , %x\n", opComma);
				if(commaam + 1>= argumentAmount){
					error("\aToo many arguments. (maximum is 4)\n", *readhead, sourcefile);
					goto endonerror;
				}
				argkinds[commaam] = kinds[0];
				commaam++;
				dummy = opComma;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				kinds[0] = '_'; kinds[1] = '_';
				prev = opComma;
				break;
			case '"':
//				printf("kind is \" %x\n", opNry);
				dummy = opNry;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				(*readhead)++;
				strtonry(&dnry, UserInput, readhead);
//				aprintnry(&dnry, Chr, true);
				/*length*/
				section = arrapp(section, u16 section, (char*) &dnry.len, 2);
				u16 section += 2;
				/*offset*/
				dummy = dnry.fst - dnry.base;
				section = arrapp(section, u16 section, (char*) &dummy, 2);
				u16 section += 2;
				/*data*/
				section = arrapp(section, u16 section, (char*)dnry.base, dnry.len);
				u16 section += (uint16_t) dnry.len;
				(*readhead)--;
				kinds[1] = kinds[0]; kinds[0] = 'p';
				prev = opNry;
				break;
			case '%':
//				printf("kind is nry %x\n", opNry);
				(*readhead)++;
				SkipSpaces(UserInput, userInputLen, readhead);
				if(!(IsNr(UserInput + *readhead) || UserInput[*readhead] == '\'' || UserInput[*readhead] == '+')){
					error("\aInvalid page declaration. Allowed characters are 0-9, -, \', +, spaces and tabs", *readhead, sourcefile);
					goto endonerror;
				}
				dummy = opNry;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				inptonry(&dnry, UserInput, readhead, globalType);
				/*length*/
				section = arrapp(section, u16 section, (char*) &dnry.len, 2);
				u16 section += 2;
				/*offset*/
				dummy = dnry.fst - dnry.base;
				section = arrapp(section, u16 section, (char*) &dummy, 2);
				u16 section += 2;
				/*data*/
//				aprintnry(&dnry, globalType, true);
				section = arrapp(section, u16 section, (char*)dnry.base, dnry.len);
				u16 section += (uint16_t) dnry.len;
				(*readhead)--;
				kinds[1] = kinds[0]; kinds[0] = 'p';
				prev = opNry;
				break;
			case '0'...'9': case '-': case '\'': case '+':
//				printf("kind is nrs %x\n", opNrs);
				dummy = opNrs;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				inptonry(&dnry, UserInput, readhead, globalType);
				/*data*/
//				aprintnry(&dnry, globalType, true);
				section = arrapp(section, u16 section, (char*)dnry.fst, typeBylen(globalType));
				u16 section += (uint16_t) typeBylen(globalType);
				(*readhead)--;
				kinds[1] = kinds[0]; kinds[0] = 'd';
				prev = opNrs;
				break;
			case 'A'...'Z':
				if(debugCompile) printf("Macro, '%s', expanded to", UserInput);
				if(insertbind(sourcefile, readhead, bindings) == NULL) goto endonerror;
				if(debugCompile) printf(": '%s'\n", UserInput);
				break;
			default:;
				char tmpcfd = UserInput[*readhead];
				if(tmpcfd >= 'a' && tmpcfd <= 'z'){
					char errorafd[] = "\aUnknown operator ' '";
					errorafd[19] = tmpcfd;
					error(errorafd, *readhead, sourcefile);
					goto endonerror;
				}
				break;
		}
		if(EndLine(UserInput + *readhead)) break;
//		printf("char: %c at %d\n", UserInput[*readhead], *readhead);
		(*readhead)++;
	} while(*readhead < userInputLen);
	argkinds[commaam] = kinds[0];

	end:
	if(!checkkinds((signed char)ins, argkinds, sourcefile))
		goto endonerror;
	freenry(&dnry);
	if(userInputLen != STANDARDuserInputLen){ UserInput = realloc(UserInput, STANDARDuserInputLen); userInputLen = STANDARDuserInputLen;};
	return section;

	endonerror:
	free(section);
	freenry(&dnry);
	return NULL;
}

bool compile(file_t* sourcefile, file_t* runfile, char* sourcename){
	UserInput = malloc(userInputLen);
	int readhead, ins, previns = 0;
	char inssection = 0;
	char* argsection = NULL;
	globalType = STANDARDtype;

	lbl_t labeling = {0, NULL, NULL, 0, NULL, NULL};
	bind_t bindings = {0, malloc(sizeof(char*)), malloc(sizeof(char*))};
	bindings.binds[0] = NULL; bindings.resos[0] = NULL;
	memowy_t includes = {0, 0, 0, NULL};
	allocmemwy(&includes, sourcename, strlen(sourcename) + 1);

	compwhile:
		readhead = 0; ins = -1;
/*get input*/
		if(mfgetsS(UserInput, userInputLen, sourcefile) == NULL)
			goto endhealthy;
//		printf("'%s'\n", UserInput);
		figins:
		inssection = 0;
		SkipSpaces(UserInput, userInputLen, &readhead);
//		printf("%c, %d\n", UserInput[readhead], readhead);
/*labels*/
		if(UserInput[readhead] == ':'){
			readhead++;
			if(previns%2 == 0 && previns/2 >= Ce && previns/2 <= Cn){
				error("\aLabel in a conditional statement", readhead-1, sourcefile);
				goto endonerror;
			}
			if(savelabel(runfile, UserInput, &readhead, &labeling, sourcefile) == NULL)
				goto endonerror;
//			printf("label: %s, %s\n", UserInput, labeling.definedlabels[labeling.labelam-1]);
//			printf("%c\n", UserInput[readhead]);
			while(UserInput[readhead] != ',' && !EndLine(UserInput + readhead)) readhead++;
			if(UserInput[readhead] == ','){
				readhead++;
				SkipSpaces(UserInput, userInputLen, &readhead);
				ins = keywordlook(UserInput, 4, typeString, &readhead);
				if(ins == -1){
					error("\aInvalid type", readhead, sourcefile);
					goto endonerror;//end
				}
				globalType = ins;
			} else globalType = STANDARDtype;
			inssection = 1 + globalType*2;
			mfapp(runfile, &inssection, 1);
			goto compwhile;
		}
/*ins*/
		if(EndLine(UserInput + readhead)){/*printf("endline, %d\n", readhead);*/ goto compwhile;}
		ins = keywordlook(UserInput, maxKeywordLen, instructionString, &readhead);
		if(ins == -1){
/*type*/
			ins = keywordlook(UserInput, 4, typeString, &readhead);
			if(ins == -1){
				error("\aInvalid keyword", readhead, sourcefile);
				goto endonerror;//end
			}
			globalType = ins;
			inssection = 1;
		}
		if(previns%2 == 0 && previns/2 >= Ce && previns/2 <= Cn && inssection == 1){
				error("\aType designation in a conditional statement", readhead, sourcefile);
				goto endonerror;//end
		}
		readhead++;
		if(inssection%2 == 0 && ins >= let && ins <= include){
//			printf("macro: %s\n", UserInput);
/*macro's*/
			switch(ins){
				case let: if(letbind(sourcefile, readhead, &bindings) == NULL) goto endonerror; break;
				case enumb:if(enumbind(sourcefile, readhead, &bindings) == NULL) goto endonerror; break;
				case include: if(includef(sourcefile, readhead, &includes) == NULL) goto endonerror; break;
			} goto compwhile;//loop
		}
/*writing ins*/
		inssection += ins*2; previns = inssection;
		mfapp(runfile, &inssection, 1);
		if(inssection%2 == 1) goto figins;//loop
		if(ins == jmp || ins == call){
			if(ins == call) globalType = STANDARDtype;
			SkipSpaces(UserInput, userInputLen, &readhead);
			if(savejmp(runfile, UserInput, &readhead, &labeling, sourcefile) == NULL)
				goto endonerror;
//			printf("jump: %s, %s\n", UserInput, labeling.definedlabels[labeling.labelam-1]);
			dummy = 0;
			mfapp(runfile, (char*) &dummy, 8);
			goto compwhile;
		} else if(ins >= Ce && ins <= Cn){
			goto figins;
		} else if(ins == ret)
			goto compwhile;
/*args*/
		argsection = buildargs(&readhead, sourcefile, &bindings, inssection/2);
		if(argsection == NULL) goto endonerror;//end
		mfapp(runfile, argsection, u16 argsection);
		free(argsection); argsection = NULL;
//		printf("Endofloop\n");
	goto compwhile;//loop

	endhealthy:
	if(solvelabels(runfile, &labeling) == -1) goto endonerror;

	free(UserInput);
	freebinds(&bindings);
	freelabels(&labeling);
	free(includes.mem);
	return true;
	endonerror:
	free(UserInput);
	freebinds(&bindings);
	freelabels(&labeling);
	free(includes.mem);
	return false;
}

#endif

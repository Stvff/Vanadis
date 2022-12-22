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
void* error(char* errormessage, int readhead, file_t* file){
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
	return NULL;
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
		good = (kinds[i] == instructionKinds[ins][i]) || (kinds[i] + 0x20 == instructionKinds[ins][i]) || ('v' == instructionKinds[ins][i]);
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
	if(quicmfptr == NULL)
		return error("", i, desfile);

	srcfile = *quicmfptr;
	mfins(desfile, desfile->pos, srcfile.mfp, srcfile.len);
	mfclose(&srcfile);
	return desfile;
}

//##################################################################################### binds

memowy_t* letbind(file_t* file, int i, memowy_t* binds){
	SkipSpaces(UserInput, userInputLen, &i);
	if(UserInput[i] < 'A' || UserInput[i] > 'Z')
		return error("\aBindings must start with a capital letter.\n", i, file);

	int begin = i;
	while(IsAlph(UserInput + i) || IsNr(UserInput + i) || UserInput[i] == '_')
		i++;

	binds->pos = allocstrmemwy(binds, UserInput + begin, i - begin);

	while(UserInput[i] != '=' && !EndLine(UserInput + i)) i++;
	if(UserInput[i] != '='){
		freememwy(binds);
		return error("\aTo assign a binding, use '='", i, file);
	}
	i++;
	SkipSpaces(UserInput, userInputLen, &i);
	if(EndLine(UserInput + i)){
		freememwy(binds);
		return error("\aNo binding", i, file);
	}
	begin = i;
	while(!EndLine(UserInput + i)) i++;
	binds->pos = allocstrmemwy(binds, UserInput + begin, i - begin);

/*	It would be proper to check if there are undefined bindings being used here.*/
/*	Then again, this way you can do def A = B[4]; def B = 2$; So that's worth something, I guess.*/
	return binds;
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

memowy_t* enumbind(file_t* file, int i, memowy_t* binds){
	/* checking the specific type of the enum */
	int enumtype = globalType;
	if(UserInput[i] == ':'){
		i++;
		enumtype = keywordlook(UserInput, 4, typeString, &i);
		if(enumtype == -1)
			return error("\aInvalid type", i, file);
		i++;
	}
	/* getting the bindbase */
	int bbase = delimstrlen(UserInput, '=');
	if(UserInput[bbase] != '=')
		return error("\aTo assign a binding, use '='", bbase, file);

	bbase++; SkipSpaces(UserInput, userInputLen, &bbase);
	int bbaselen = strlen(UserInput + bbase);
	if(bbaselen == 0)
		return error("\aNo binding", bbase + bbaselen, file);

	char whichref[5] = {' ',
						' ',
						' ',
						UserInput[bbase + bbaselen - 1] == '<'?'>':']',
						'\0'};
	/* saving the bindbase */
	memowy_t memowy = {0, 0, 0, NULL};
	allocmemwy(&memowy, UserInput + bbase, bbaselen);
	appmemwy(&memowy, whichref, 5);
	/* splitting on spaces and making the bindings themselves */
	int bindlen;
	short bindcn = 0;
	SkipSpaces(UserInput, userInputLen, &i);
	do{
		if(UserInput[i] < 'A' || UserInput[i] > 'Z'){
			free(memowy.mem);
			return error("\aBindings must start with a capital letter.\n", i, file);;
		}
		bindlen = delimstrlen(UserInput + i, ' ');
		/* setting the binding */
		binds->pos = allocstrmemwy(binds, UserInput + i, bindlen);
		/* setting the base as that binding's resolution */
		binds->pos = allocmemwy(binds, memowy.mem + 2, lenwy(memowy.mem));
		printinttostr(data(binds) + lengf(binds) - 5,
					  bindcn*typeBylen(enumtype));
		/* get ready for next binding */
		i += bindlen;
		SkipSpaces(UserInput, userInputLen, &i);
		bindcn++;
	} while(UserInput[i] != '=');

	free(memowy.mem);
	return binds;
}

memowy_t* insertbind(file_t* file, int* readhead, memowy_t* binds){
	int i = *readhead;
	int l = i;
	while(IsAlph(UserInput + l) || IsNr(UserInput + l) || UserInput[l] == '_')
		l++;
	l -= i;

	indexmemwy(binds, -1);
	pos_t bi = sfindmemwy(binds, allocstrmemwy(binds, UserInput + i, l), -2);
	freememwy(binds);
	if(bi == binds->avai)
		return error("\aUndefined binding", i, file);
	pos_t ol = binds->pos;
	binds->pos = bi;

//	indexmemwy(binds, -1);
	int blen = lengf(binds) - 1;
	indexmemwy(binds,  1);
	int rlen = lengf(binds) - 1;
	userInputLen = STANDARDuserInputLen + rlen - blen;
	if(userInputLen > STANDARDuserInputLen)
		UserInput = realloc(UserInput, userInputLen);

	memmove(UserInput + *readhead + rlen,
			UserInput + *readhead + blen,
			STANDARDuserInputLen - *readhead - blen - 1);
	memcpy(UserInput + *readhead, data(binds), rlen);
	(*readhead)--;

	binds->pos = ol;
	indexmemwy(binds,  1);
	return binds;
}

memowy_t* opennamespace(memowy_t* binds){
	dummy = 0x0101;
	allocmemwy(binds, &dummy, 2);
	binds->pos = allocmemwy(binds, &dummy, 2);
	return binds;
}

memowy_t* closenamespace(memowy_t* binds){
	dummy = 0x0101;
	pos_t prevspace = sfindmemwy(binds, allocmemwy(binds, &dummy, 2), -2);
	freememwy(binds);
	if(prevspace == binds->avai)
		prevspace = 0;

	while(binds->pos != prevspace){
		indexmemwy(binds, -1);
		freememwy(binds);
	}
	return binds;
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
		return error("\aLabel can not be 0 characters long, allowed characters are: a-z, A-Z, 0-9, _ and -.\n", i, src);
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
		return error("\aLabel can not be 0 characters long, allowed characters are: a-zA-Z0-9_-\n", i, src);
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
		memcpy(file->mfp + labels->jumppos_s[i], labels->labelpos_s+ labelnr, sizeof(uint64_t));
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

char* arrapp(char* des, uint64_t deslen, void* src, uint64_t srclen){
	des = realloc(des, deslen + srclen);
	memcpy(des + deslen, src, srclen);
	return des;
}

//##################################################################################### building, compiling

ptr_t buildargs(int* readhead, file_t* sourcefile, memowy_t* bindings, char ins){
//	printf("buildargs\n");
//	bool macrowas = false; // this is for debugCompile
	ptr_t section = {malloc(2)};
	*section.u16 = 2;
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
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.u16)++;
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
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.u16)++;
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
					*(section.u8 - 1 + *section.u16) += 2;
					kinds[0] = 'D';
					prev += 2;
					break;
				}
				dummy = opImm;
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.u16)++;
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
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.u16)++;
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
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.u16)++;
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
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.u16)++;
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
					*(section.u8 - 1 + *section.u16) += 2;
					prev += 2;
				} break;
			case 'l':
//				printf("op is l %x\n", opLength);
				dummy = opLength;
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.chr)++;
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
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.u16)++;
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
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.u16)++;
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
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.u16)++;
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
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.u16)++;
				kinds[0] = '_'; kinds[1] = '_';
				prev = opComma;
				break;
			case '"':
//				printf("kind is \" %x\n", opNry);
				dummy = opNry;
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.u16)++;
				(*readhead)++;
				strtonry(&dnry, UserInput, readhead);
//				aprintnry(&dnry, Chr, true);
				/*length*/
				section.chr = arrapp(section.chr, *section.u16, &dnry.len, 2);
				*section.u16 += 2;
				/*offset*/
				dummy = dnry.fst.u8 - dnry.base.u8;
				section.chr = arrapp(section.chr, *section.u16, &dummy, 2);
				*section.u16 += 2;
				/*data*/
				section.chr = arrapp(section.chr, *section.u16, dnry.base.chr, dnry.len);
				*section.u16 += (uint16_t) dnry.len;
				(*readhead)--;
				kinds[1] = kinds[0]; kinds[0] = 'p';
				prev = opNry;
				break;
			case '0'...'9': case '-': case '\'': case '+':
				inptonry(&dnry, UserInput, readhead, globalType);
				if(dnry.len / typeBylen(globalType) == 1){
//					printf("kind is nrs %x\n", opNrs);
					dummy = opNrs;
					section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
					(*section.u16)++;
					/*data*/
					section.chr = arrapp(section.chr, *section.u16, dnry.fst.chr, typeBylen(globalType));
					*section.u16 += (uint16_t) typeBylen(globalType);
					(*readhead)--;
					kinds[1] = kinds[0]; kinds[0] = 'd';
					prev = opNrs;
				} else {
//					printf("kind is nry %x\n", opNry);
					dummy = opNry;
					section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
					(*section.u16)++;
					/*length*/
					section.chr = arrapp(section.chr, *section.u16, &dnry.len, 2);
					*section.u16 += 2;
					/*offset*/
					dummy = dnry.fst.u8 - dnry.base.u8;
					section.chr = arrapp(section.chr, *section.u16, &dummy, 2);
					*section.u16 += 2;
					/*data*/
//					aprintnry(&dnry, globalType, true);
					section.chr = arrapp(section.chr, *section.u16, dnry.base.chr, dnry.len);
					*section.u16 += (uint16_t) dnry.len;
					(*readhead)--;
					kinds[1] = kinds[0]; kinds[0] = 'p';
					prev = opNry;
				}
				break;
			case 'A'...'Z':
				if(insertbind(sourcefile, readhead, bindings) == NULL) goto endonerror;
//				macrowas = true;
				break;
			case 'b':
				dummy = opBrk;
				section.chr = arrapp(section.chr, *section.u16, &dummy, 1);
				(*section.u16)++;
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
	if(debugCompile){
		printf("compiled: '%s'\n", UserInput);
	}

	freenry(&dnry);
	if(userInputLen != STANDARDuserInputLen){ UserInput = realloc(UserInput, STANDARDuserInputLen); userInputLen = STANDARDuserInputLen;};
	return section;

	endonerror:
	free(section.u8);
	freenry(&dnry);
	return (ptr_t) NULL;
}

bool compile(file_t* sourcefile, file_t* runfile, char* sourcename){
	UserInput = malloc(userInputLen);
	int readhead, ins, previns = 0;
	char inssection = 0;
	ptr_t argsection = {NULL};
	globalType = STANDARDtype;

	lbl_t labeling = {0, NULL, NULL, 0, NULL, NULL};
	memowy_t bindings = {0, 0, 0, NULL};
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
		if(inssection%2 == 0 && ins >= def && ins <= insert){
//			printf("macro: %s\n", UserInput);
/*macro's*/
			switch(ins){
				case def: if(letbind(sourcefile, readhead, &bindings) == NULL) goto endonerror; break;
				case enumb:if(enumbind(sourcefile, readhead, &bindings) == NULL) goto endonerror; break;
				case opens: if(opennamespace(&bindings) == NULL) goto endonerror; break;
				case clons: if(closenamespace(&bindings) == NULL) goto endonerror; break;
				case insert: if(includef(sourcefile, readhead, &includes) == NULL) goto endonerror; break;
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
			readhead++; goto figins;
		} else if(ins == ret)
			goto compwhile;
/*args*/
		argsection = buildargs(&readhead, sourcefile, &bindings, inssection/2);
		if(argsection.chr == NULL) goto endonerror;//end
		mfapp(runfile, argsection.p, *argsection.u16);
		free(argsection.p); argsection.p = NULL;
//		printf("Endofloop\n");
	goto compwhile;//loop

	endhealthy:
	if(solvelabels(runfile, &labeling) == -1) goto endonerror;

	free(UserInput);
	freelabels(&labeling);
	clearmemwy(&bindings);
	clearmemwy(&includes);
	return true;
	endonerror:
	free(UserInput);
	freelabels(&labeling);
	clearmemwy(&bindings);
	clearmemwy(&includes);
	return false;
}

#endif

#include "nry.h"
#include "vic.h"
#include "iostr.h"
#include "switches.h"
#include "user.h"

char* arrapp(char* des, uint64_t deslen, char* src, uint64_t srclen){
	des = realloc(des, deslen + srclen);
	memcpy(des + deslen, src, srclen);
	return des;
}

char* buildargs(int* readhead, file_t* sourcefile, bind_t* bindings, char ins){
//	printf("buildargs\n");
	char* section = malloc(2);
	u16 section = 2;
	nry_t dnry; makenry(&dnry, 8);
	signed char commaam = 0;
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
				break;
			case '!':
//				printf("op is ! %x\n", opImm);
				dummy = opImm;
				section = arrapp(section, u16 section, (char*) &dummy, 1);
				(u16 section)++;
				if(kinds[0]!='p' && kinds[0]!='P'){
					error("\aOperator '!' expects a page, not a datum or nothing.\n", *readhead, sourcefile);
					goto endonerror;
				}
				kinds[0] = kinds[0]=='P'?'D':'d';
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
				break;
			case '*':
//				printf("op is *\n");
				if(u8(section - 1 + u16 section) == opEntry || u8(section - 1 + u16 section) == opRef){
					if(kinds[1]!='P'){
						error("\aThis operator expects its first argument to be a mutable page.\n", *readhead, sourcefile);
						goto endonerror;
					}
					u8 (section - 1 + u16 section) += 2;
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
				break;
			case 'A'...'Z':
//				printf("Kind is binding, %c\n%s\n", UserInput[*readhead], UserInput);
				if(insertbind(sourcefile, readhead, bindings) == NULL) goto endonerror;
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
/*writing ins*/
		if(inssection%2 == 0 && ins >= let && ins <= include){
//			printf("macro: %s\n", UserInput);
			switch(ins){
				case let: if(letbind(sourcefile, readhead, &bindings) == NULL) goto endonerror; break;
				case enumb:if(enumbind(sourcefile, readhead, &bindings) == NULL) goto endonerror; break;
				case include: if(includef(sourcefile, readhead, &includes) == NULL) goto endonerror; break;
			} goto compwhile;//loop
		}
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


#define exprbufflen 256
bool run(file_t* runfile){
	bool retbool = true;
	globalType = STANDARDtype;
	char head;
	bool insornot;
	uint16_t exprlen;
	nry_t callnr; makenry(&callnr, 16);

	nry_t* args[argumentAmount] = {0};
	uint8_t* nrs[argumentAmount] = {0};

	nry_t allp[argumentAmount*2] = {0};
	uint64_t alld[argumentAmount*2] = {0};

	while(runfile->pos < runfile->len && retbool){
		head = runfile->mfp[runfile->pos];
		runfile->pos++;
		insornot = head%2;
		head /= 2;
		if(insornot){
//			printf("Type is now %s\n", typeString[(signed char)head]);
			globalType = head;
		} else switch(head){
			case Ce: if(!flag.c.e) goto skip; break;
			case Cs: if(!flag.c.s) goto skip; break;
			case Cg: if(!flag.c.g) goto skip; break;
			case Cse:if(!(flag.c.s || flag.c.e)) goto skip; break;
			case Cge:if(!(flag.c.g || flag.c.e)) goto skip; break;
			case Cn: if(flag.c.e) goto skip; break;
				skip: if(runfile->pos < runfile->len){
					head = runfile->mfp[runfile->pos]/2;
					if(head == call || head == jmp) runfile->pos += 9;
					else if(head == ret){ runfile->pos += 1; globalType = STANDARDtype;}
					else runfile->pos += (u16 (runfile->mfp + runfile->pos + 1)) + 1;
//					printf("skip, runfile->pos: %lx\n", runfile->pos);
				} break;
			case call:
				i64 callnr.base = stackFrameOffset;
				u64 (callnr.base + 8) = runfile->pos + 8;
				pushtost(&callnr); Flip();
				stackFrameOffset = stackPtr + 1;
//				printf("call, stackFrameOffset: x%lx, runfile->pos: %lx\n", stackFrameOffset, runfile->pos);
			case jmp:
//				globalType = STANDARDtype;
				runfile->pos = u64 (runfile->mfp + runfile->pos);
//				printf("jmp, runfile->pos: x%lx\n", runfile->pos);
				break;
			case ret:
				Unflip();
				popfromst(&callnr);
				stackFrameOffset = i64 callnr.base;
				runfile->pos = u64 (callnr.base + 8);
				globalType = STANDARDtype;
//				printf("ret: stackFrameOffset: x%lx, runfile->pos: %lx\n", stackFrameOffset, runfile->pos);
				break;
			default:
//				printf("default, ins: %s, x%x (multiplied by 2 in bin)\n", instructionString[(signed char)head], head);
				exprlen = u16 (runfile->mfp + runfile->pos);
//				printf("expressiontime:\n");
				if(!evalexpr(runfile->mfp + runfile->pos, exprlen, args, nrs, allp, alld)){retbool = false; break;}
//				printf("executiontime\nP0: %lx, P1: %lx, P2: %lx, P3: %lx\nD0: %lx, D1: %lx, D2: %lx, D3: %lx\n",
//					(uint64_t) args[0], (uint64_t) args[1], (uint64_t) args[2], (uint64_t) args[3],
//					(uint64_t) nrs[0], (uint64_t) nrs[1], (uint64_t) nrs[2], (uint64_t) nrs[3]);
				if(!execute(head, args, nrs)){retbool = false; break;}
				runfile->pos += exprlen;
				break;
		}
//		printf("pos x%lx\n", runfile->pos);
	}
	freenry(&callnr);
	return retbool;
}

enum commandlineargsstatemachine {SOURCE_IN = 1, BINARY_IN = 2, STACKARGS = 4, BINARY_OUT = 8, RUN = 16};
int RETURN = 0;
#define ENDonERROR {RETURN = 1; goto endonerror;}

union {
	char te; struct {
	bool source_in:1;
	bool binary_in:1;
	bool stackargs:1;
	bool binary_out:1;
	bool run:1;
	} t;
} sta;

int main(int argc, char** argv){
	file_t sourcefile = {0, 0, NULL};
	file_t runfile = {0, 0, NULL};
	sta.te = SOURCE_IN | RUN;
	nry_t stackarg = {NULL, 0, NULL};
	int desf = -1;
	int sourcenr = 0;
	for(int i = 1; i < argc; i++){
		if(sta.t.stackargs){
			pushtost(strcpytonry(&stackarg, argv[i]));
			freenry(&stackarg);
		} else if(argv[i][0] == '-') switch (argv[i][1]){
			case 'r': sta.te = BINARY_IN | RUN; break;
			case 'c': sta.te = SOURCE_IN | BINARY_OUT; break;
			case 'o': sta.te |= BINARY_OUT; desf = 0; break;
			case 'e': sta.te = SOURCE_IN; break;
//			case 'h': break;
			default: printf("%s\n", argv[i]); break;
		} else if(sta.t.source_in){
			quicmfptr = mfopen(argv[i], &sourcefile);
			if(quicmfptr == NULL) ENDonERROR;
			sourcenr = i;
			sourcefile = *quicmfptr;
			sta.te |= STACKARGS;
			initmac();
		} else if(sta.t.binary_in){
			quicmfptr = mfopen(argv[i], &runfile);
			if(quicmfptr == NULL) ENDonERROR;
			runfile = *quicmfptr;
			sta.te |= STACKARGS;
			initmac();
		} else if(sta.t.binary_out && desf == 0){
			desf = i;
			printf("A\n");
		}
	}
	if(sta.t.source_in){
		if(!compile(&sourcefile, &runfile, argv[sourcenr])) ENDonERROR;
	}
	mfclose(&sourcefile);
	if(sta.t.binary_out){
		printf("filelen: %ld\n", runfile.len);
		if(desf > 0) quicfptr = fopen(argv[desf], "wb");
		else quicfptr = fopen("out.vco", "wb");
		fwrite(runfile.mfp, 1, runfile.len, quicfptr);
		fclose(quicfptr);
	}
	if(sta.t.run){
		if(!run(&runfile)) ENDonERROR;
	}

	endonerror:
	mfclose(&sourcefile);
	mfclose(&runfile);
	return RETURN;
}

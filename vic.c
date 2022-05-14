#include "vic.h"
#include <time.h>

bool dothing(char* userInput, file_t* file, char* mode){
	nry_t* args[argumentAmount];
	nry_t temps[argumentAmount];
	bool stackref[argumentAmount];
	for(int i = 0; i < argumentAmount; i++){
		args[i] = &temps[i];
		args[i]->len = 1;
		stackref[i] = false;
	}

	bool filerelated = false;
	int form = (UN regs[formr].base)%4;
	uint8_t argNr = 0;
	int insNr = -2;
	int readhead = 0;
	int p = 0;
	while(userInput[p] != '|' && !EndLine(&userInput[p])){
		if(IsSpace(&userInput[p])){
			if(insNr == -1) break;
		} else if(insNr == -1 || insNr == -2){
			insNr = strlook(userInput, maxKeywordLen, instructionString, &p);
//			printf("instr: %d\n", insNr);
			readhead = p + 1;
			if(insNr != -1){ switch(insNr){
				case endprog: return false; break;
				case Ce: if(SN regs[flag].base == 0) insNr = -2; else return true; break;
				case Cg: if(SN regs[flag].base == 1) insNr = -2; else return true; break;
				case Cs: if(SN regs[flag].base == 2) insNr = -2; else return true; break;
				case Cn: if(SN regs[flag].base != 0) insNr = -2; else return true; break;
				case rmr ... rjmp: filerelated = true; break;
			}}
		}
		p++;
		if(userInput[p] == '$') stackref[argNr] = true;
		if(userInput[p] == ',') argNr++;
		if(userInput[p] == '+') temps[argNr].len++;
		if(userInput[p] == '|') form = format(userInput, p);
	}
	if(insNr == -2) return true;
	if(insNr == -1){
		printf("Invalid instruction.\n");
		return true;
	}
//	printf("form: %d\n", form);

	if(form%2 == 1){ for(int i = 0; i < argumentAmount; i++) args[i]->len *= 8; }
	for(int i = 0; i < argumentAmount; i++){
//		printf("length of arg %d is %lu", i, args[i]->len);
		makenry(&temps[i], temps[i].len);
//		printf(", base is: %lx\n",  (int64_t) args[i]->base);
		memset(temps[i].base, 0, temps[i].len);
	}

	int regNr = -2;
	argNr = 0;
	uint64_t digNr = 0;
	uint64_t dig = 0;
	uint64_t inc = 1 + 7*(form%2);
	while(userInput[readhead] != '|' && !EndLine(&userInput[readhead])){
//numbers
		if(IsNr(&userInput[readhead])){
			dig = inputtoint(userInput, readhead, &readhead, true);
//			printf("dig: %lu\n", dig);
			switch(form){
				case 0: SN (args[argNr]->base + digNr) = (int8_t) dig; break;
				case 1: SL (args[argNr]->base + digNr) = (int64_t) dig; break;
				case 2: UN (args[argNr]->base + digNr) = (uint8_t) dig; break;
				case 3: UL (args[argNr]->base + digNr) = (uint64_t) dig; break;
			}
		}
// registers
		if(IsAlph(&userInput[readhead])){
			regNr = strlook(userInput, maxKeywordLen, registerString, &readhead);
//			printf("entry: %c, register: %d\n", userInput[readhead], regNr);
			if(regNr != -1){
				args[argNr] = &regs[regNr];
				if(regNr >= stkptr) switch (regNr){
					case stkptr: inttonry(&regs[stkptr], stackPtr, 1); break;
					case cdxptr: inttonry(&regs[cdxptr], codexPtr, 1); break;
					case tme: time_t thetime = time(&thetime); inttonry(&regs[tme], (uint64_t)thetime, form); break;
				}
			} else {
				printf("Invalid register on argument %d.\n", argNr);
				goto endsafe;
			}
		}
// strings
		if(userInput[readhead] == '"'){
//			printf("string, r: %d\n", readhead);
			readhead++;
			strtonry(args[argNr], userInput, &readhead);
		}
// fst
		if(userInput[readhead] == '\'') args[argNr]->fst = args[argNr]->base + digNr;
// args
		if(userInput[readhead] == ','){argNr++; digNr = 0;}
// digs
		if(userInput[readhead] == '+') digNr += inc;
		readhead++;
//		printf("readhead: %d, entry: %c\n", readhead, userInput[readhead]);
	}

	for(int i = 0; i < argumentAmount; i++){
//		printf("arg %d: ", i); aprintnry(args[i], form, true);
		if(stackref[i]){
			int64_t acc = 0; switch(form){
				case 0: acc = SN args[i]->fst; break;
				case 1: acc = SL args[i]->fst; break;
				case 2: acc = UN args[i]->fst; break;
				case 3: acc = UL args[i]->fst; break;}
			acc += SL regs[offset].fst;
			if(acc < 0 || acc > stackPtr){
				printf("Tried to access a non-existant element (entry %ld) on the stack.\n", acc);
				printf("offset: %ld, stkptr: %ld\n", SL regs[offset].fst, stackPtr);
				goto endsafe;
			}
			args[i] = stack[acc];
		}
	}

	bool doprint = false;
	if(*mode == 't') doprint = true;
	if(filerelated){
		if(*mode == 'f') filerelexec(insNr*4 + form, args, file);
		else printf("This instruction only works in a script.\n");
	} else {
		nry_t* retptr = exec(insNr*4 + form, args, &doprint);
		if(doprint){
			if(insNr < print) aprintnry(retptr, form, true);
			copynry(&regs[ans], retptr);
		}
	}

	endsafe:
	for(int i = 0; i < argumentAmount; i++) freenry(&temps[i]);
	return true;
}

bool getinput(char* userInput, file_t* file, char* mode){
	switch(*mode){
		case 't':
			printf("<> ");
			if(fgets(userInput, userInputLen, stdin) == NULL){
				printf("\n");
				return false;
			}
			break;
		case 'e':
			while(fgets(userInput, userInputLen, stdin) != NULL){
				printf("-- ");
				file->mfp = realloc(file->mfp, file->len + userInputLen);
				strcat(file->mfp, userInput);
			}
		case 'f':
			if(mfgets(userInput, userInputLen, file) == NULL){
//				printf("\n");
				return false;
			}
			break;
	}
	return true;
}

int main(int argc, char** argv){
	char* userInput = malloc(sizeof(char[userInputLen + userInputLen]));
	for(int i = 0; i < regAmount; i++){
		makenry(&regs[i], 8);
		memset(regs[i].base, 0, 8);
	}
	bool running = true;
	char mode = 't';
	if(!initmac()) return -1;
	file_t file;

	for(int i = 1; i < argc; i++){
		if(mfopen(argv[i], &file) != NULL){
			mode = 'f';
		} else running = false;
	}

	while(running){
		if(!getinput(userInput, &file, &mode)) break;
//		printf("userInput: %s", userInput);
		if(!dothing(userInput, &file, &mode)) break;;
	}

	if(mode == 'f'){
		mfclose(&file);
	}
	freemac();
	for(int i = 0; i < regAmount; i++) freenry(&regs[i]);
	free(userInput);
	return 0;
}

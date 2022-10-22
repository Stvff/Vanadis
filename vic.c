#include"va_compile.h"
#include"va_run.h"

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
	nry_t stackarg = {NULL, 0, NULL};
	int sourcenr = 0;
	int binaryoutnr = 0;
//	bool debug = false;
	sta.te = 0;
	if(!initmac()) ENDonERROR;
	for(int i = 1; i < argc; i++){
		if(sta.t.stackargs){
			if(debugIns) printf("push %s\n", argv[i]);
			pushtost(strcpytonry(&stackarg, argv[i]));
			freenry(&stackarg);
		} else if(argv[i][0] == '-') switch (argv[i][1]){
			case 'c': sta.te = SOURCE_IN | BINARY_OUT; break;
			case 'b': sta.te = BINARY_IN | RUN; break;
			case 't': sta.te = SOURCE_IN; break;
			case 'd': switch (argv[i][2]) { 
				case 'i': debugIns = true; break;
				case 'e': debugExpr = true; break;
				case 'c': debugCompile = true; break;
				case 'b': debugEnters = true; break;
				default: debugIns = true; debugExpr = true; debugCompile = true; break;}
				break;
			case 'D': debugIns = true; debugExpr = true; debugCompile = true; debugEnters = true; break;
			default:
				printf("Vanadis Interpreter and Compiler\n");
				printf("Usage: $ vic [options] <files> [vanadis machine stack arguments]\n\n");
				printf("	Options:\n");
				printf("		-c <source file> <target file> : Compiles source script and saves the result to the target file.\n");
				printf("		-b <binary file>               : Runs the binary file.\n");
				printf("		-t <source file>               : Tests for errors in the source script.\n");
				printf("		-d                             : Turns on debugging.\n");
				printf("		-D                             : Turns on step-through debugging.\n");
				printf("\nIf no options are given, Vanadis will look at the extension of\n");
				printf("the given file to determine what the appropriate course of action is.\n\n");
				break;
		} else {
			if(sta.te == 0){
				dummy = strlen(argv[i]);
				if(debugIns) printf("No options\n");
				if(dummy > 3 && argv[i][dummy-3] == 'v' && argv[i][dummy-2] == 'c' && argv[i][dummy-1] == 'o'){
					//runfile
					if(debugIns) printf("bin\n");
					quicmfptr = mfopen(argv[i], &runfile);
					if(quicmfptr == NULL) ENDonERROR;
					sta.t.binary_in = true;
				} else {
					//sourcefile
					if(debugIns) printf("src\n");
					quicmfptr = mfopen(argv[i], &sourcefile);
					if(quicmfptr == NULL) ENDonERROR;
					sta.t.source_in = true;
					sta.t.binary_in = true;
					sourcenr = i;
				}
				sta.t.stackargs = true;
				sta.t.run = true;
			} else if(sta.t.source_in) {
				if(debugIns) printf("source in\n");
				quicmfptr = mfopen(argv[i], &sourcefile);
				if(quicmfptr == NULL) ENDonERROR;
				sta.t.source_in = true;
				sourcenr = i;
				if(sta.t.binary_out && i + 1 < argc ) binaryoutnr = ++i;
				sta.t.stackargs = true;
			} else if(sta.t.binary_in){
				if(debugIns) printf("binary in\n");
				quicmfptr = mfopen(argv[i], &runfile);
				if(quicmfptr == NULL) ENDonERROR;
				sta.t.binary_in = true;
				sta.t.stackargs = true;
			}
		}
	}
	if(sta.t.source_in){
		if(debugIns) printf("compile\n");
		if(!compile(&sourcefile, &runfile, argv[sourcenr])) ENDonERROR;
	}
	mfclose(&sourcefile);
	if(sta.t.binary_out){
		if(debugIns) printf("write %s\n", argv[binaryoutnr]);
		if(binaryoutnr > 0) quicfptr = fopen(argv[binaryoutnr], "wb+");
		else quicfptr = fopen("out.vco", "wb+");
		if(quicfptr == NULL){
			printf("Vanadis: Could not write to file to save bytecode.\n");
			RETURN = 1; goto endonerror;
		}
		fwrite(runfile.mfp, 1, runfile.len, quicfptr);
		fclose(quicfptr);
	}
	if(sta.t.run){
		if(debugIns) printf("run\n");
		if(!run(&runfile)) ENDonERROR;
	}

	freemac();
	endonerror:
	mfclose(&sourcefile);
	mfclose(&runfile);
	return RETURN;
}

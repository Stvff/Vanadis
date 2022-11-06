#include "util_nry.h"
#include "va_vic.h"
#include <math.h>

typedef struct {
	int mglobalType;
	nry_t** mstack;
	nry_t** mcodex;
	int64_t mstackPtr;
	int64_t mcodexPtr;
	int64_t mstackFrameOffset;
	bool ret;
} VM;

VM libraryfunctionexposedtoVanadis(VM machine, nry_t** args, uint8_t** nrs){
	//printf("hello from the other side %p %p %p\n", &machine, args, nrs);
	if(args[0] != NULL) switch(chr args[0]->fst){
		case 's':
			switch(chr (args[0]->fst + 1)){
				case '3':
				f32 nrs[1] = sqrtf(f32 nrs[1]);
				break;
				case '6':
				f64 machine.mstack[machine.mstackPtr-1]->fst = sqrt(f64 machine.mstack[machine.mstackPtr-1]->fst);
				printf("f64\n");
				break;
			}
			break;
		default:
			fprintf(stderr, "No-op in library file\n");
			machine.ret = false;
			break;
	}
	return machine;
}

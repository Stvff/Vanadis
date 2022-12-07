#include "util_nry.h"
#include "va_vic.h"
#include <math.h>
#include <signal.h>

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
			switch(chr ((args[0]->fst) + 1)){
				case 'q':
					if(machine.mglobalType == F64)
						f64 nrs[1] = sqrt(f64 nrs[1]);
					else
						f32 nrs[1] = sqrtf(f32 nrs[1]);
					break;
				case 'y':
//				while (true) {
//				aprintnry(args[1], Chr, 0);
				int ret = system((char*) (args[1]->fst) );
				if(ret == -1) machine.ret = false;
//					if (WIFSIGNALED(ret) &&
//						(WTERMSIG(ret) == SIGINT || WTERMSIG(ret) == SIGQUIT))
//						break;
//				}
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

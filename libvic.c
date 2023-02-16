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

enum {Sqrt_f32 = 1, Sqrt_f64, Sin_f32, Sin_f64, Cos_f32, Cos_f64, Tan_f32, Tan_f64, Pi_f32, Pi_f64};

VM libraryfunctionexposedtoVanadis(VM machine, nry_t* args[argumentAmount], ptr_t nrs[argumentAmount]){
	//printf("hello from the other side %p %p %p\n", &machine, args, nrs);
	if(nrs[0].p != NULL) switch(integer(nrs[0], machine.mglobalType)){
		case Sqrt_f32:
//			printf("sqrt3\n");
			*nrs[1].f32 = sqrtf(*nrs[2].f32);
			break;
		case Sin_f32:
//			printf("sin3\n");
			*nrs[1].f32 = sinf(*nrs[2].f32);
			break;
		case Cos_f32:
//			printf("cos3\n");
			*nrs[1].f32 = cosf(*nrs[2].f32);
			break;
		case Tan_f32:
//			printf("tan3\n");
			*nrs[1].f32 = tanf(*nrs[2].f32);
			break;
		case Sqrt_f64:
//			printf("sqrt6\n");
			*nrs[1].f64 = sqrt(*nrs[2].f64);
			break;
		case Sin_f64:
//			printf("sin6\n");
			*nrs[1].f64 = sin(*nrs[2].f64);
			break;
		case Cos_f64:
//			printf("cos6\n");
			*nrs[1].f64 = cos(*nrs[2].f64);
			break;
		case Tan_f64:
//			printf("tan6\n");
			*nrs[1].f64 = tan(*nrs[2].f64);
			break;
		case Pi_f32:
			*nrs[1].f32 = M_PI;
			break;
		case Pi_f64:
			*nrs[1].f64 = M_PI;
			break;
		default:
			fprintf(stderr, "No-op in library\n");
			machine.ret = false;
			break;
	}
	return machine;
}

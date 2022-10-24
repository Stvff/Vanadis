#ifndef VA_VIC_H
#define VA_VIC_H
#define maxKeywordLen 16
#define STANDARDuserInputLen 512
#define argumentAmount 4

// ######################################################################################## instructions
char instructionString[][maxKeywordLen] = {
	"alloc", "free", "push", "pop", "flip", "unf", // 1
	"staptr", "cdxptr", // 2 
	"mov", "set", "memv", "fill", "rsz", // 3
	"inc", "dec", "add", "sub", "mul", "div", "mod", // 4
	"and", "or", "xor", "not", "rshf", "lshf", // 5
	"gc", "sc", "gec", "sec", "ec", "cmp", "pec", // 6
	"input", "dprint", "print", "lib", // 7
	"fread", "fwrite", "flen", "time", // 8
	"ex", // 9
	"Ce", "Cs", "Cg", "Cse", "Cge", "Cn", "jmp", "call", "ret", // 10
	"def", "enum", "import",// 12
	"\0end"
};

enum instructionEnum {
	allocst, freest, push, pop, flip, unf, // 1
	staptr, cdxptr, // 2
	mov, set, memv, fill, rsz, // 3
	inc, dec, add, sub, mul, divi, modu, // 4
	and, or, xor, not, rshf, lshf, // 5
	gc, sc, gec, sec, ec, cmp, pec, // 6
	input, dprint, print, lib, // 7
	firead, fiwrite, flen, timei, // 8
	ex, // 9
	Ce, Cs, Cg, Cse, Cge, Cn, jmp, call, ret, // 10
	let, enumb, include,// 11
	final
};

char instructionKinds[][argumentAmount+1] = {
	"d___", "d___", "p___", "P___", "d___", "d___", // 1
	"D___", "D___", // 2
	"Pp__", "Dd__", "Ppdd", "Pdd_", "Pd__", // 3
	"D___", "D___", "Ddd_", "Ddd_", "Ddd_", "Ddd_", "Ddd_", // 4
	"Ddd_", "Ddd_", "Ddd_", "Dd__", "Ddd_", "Ddd_", // 5
	"dd__", "dd__", "dd__", "dd__", "dd__", "dd__", "pp__", // 6
	"Ppd_", "dd__", "pd__", "PPPp", // 7
	"PpdD", "ppdD", "DpD_", "D___", // 8
	"d___" // 9
};

char operationString[] = {
	'_', '$', '@', 'B', 'T', '!', '^', ']', '>', '*', '*', 'l', 'o', 't', '~', ',', '%', 'N', 'b' };
enum operationEnum {
	opNoop, opStackref, opStackrevref, opStackrefImm, opStackrevrefImm, opImm, opMakenry, opEntry, opRef, opEntryKeep, opRefKeep,
	opLength, opOffset, opSizeof,
	opSwap, opComma, opNry, opNrs, opBrk
};

char typeString[][4] = {
	"chr", "i8", "u8",
	"i16", "u16", "i32", "u32",
	"i64", "u64", "f32", "f64", "\0end"
};

enum compareEnum {
	CE = 1, CG = 2, CS = 4,
};

#endif

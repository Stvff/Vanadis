#ifndef VA_VIC_H
#define VA_VIC_H
#define maxKeywordLen 16
//#define STANDARDuserInputLen 512
#define argumentAmount 4

// ######################################################################################## instructions
char instructionString[][maxKeywordLen] = {
	"alloc", "free", "push", "pop", "flip", "unf", // 1
	"staptr", "cdxptr", // 2 
	"mov", "set", "cast", "memv", "fill", "rsz", // 3
	"inc", "dec", "add", "sub", "mul", "div", "mod", // 4
	"and", "or", "xor", "not", "rshf", "lshf", // 5
	"gcmp", "scmp", "gecmp", "secmp", "ecmp", "cmp", "pecmp", // 6
	"input", "dprint", "print", "lib", // 7
	"fread", "fwrite", "flen", "time", // 8
	"ex", // 9
	"Ce", "Cse", "Cge", "Cs", "Cg", "Cn", "jmp", "call", "ret", // 10
	"def", "enum", "opens", "clons", "insert",// 12
	"\0end"
};

enum instructionEnum {
	allocst, freest, push, pop, flip, unf, // 1
	staptr, cdxptr, // 2
	mov, set, cast, memv, fill, rsz, // 3
	inc, dec, add, sub, mul, divi, modu, // 4
	and, or, xor, not, rshf, lshf, // 5
	gcmp, scmp, gecmp, secmp, ecmp, cmp, pecmp, // 6
	input, dprint, print, lib, // 7
	firead, fiwrite, flen, timei, // 8
	ex, // 9
	Ce, Cse, Cge, Cs, Cg, Cn, jmp, call, ret, // 10
	def, enumb, opens, clons, insert,// 11
	final
};

char instructionKinds[][argumentAmount+1] = {
	"d___", "d___", "p___", "P___", "d___", "d___", // 1
	"D___", "D___", // 2
	"Pp__", "Dd__", "Ddd_", "Ppdd", "Pdd_", "Pd__", // 3
	"D___", "D___", "Ddd_", "Ddd_", "Ddd_", "Ddd_", "Ddd_", // 4
	"Ddd_", "Ddd_", "Ddd_", "Dd__", "Ddd_", "Ddd_", // 5
	"dd__", "dd__", "dd__", "dd__", "dd__", "dd__", "pp__", // 6
	"Ppd_", "dd__", "pd__", "vvvv", // 7
	"PpdD", "ppdD", "DpD_", "D___", // 8
	"vvvv" // 9
};

char operationString[][3] = {
	"_", "$", "@", // 1
	"$!", "@!", "!", // 2
	"^", "]", ">", "]*", ">*", // 3
	"l", "o", "t", // 4
	"~", ",", // 5
	"Pa", "Da", "b" // 6
};

enum operationEnum {
	opNoop,        opStackref,       opStackrevref, // 1
	opStackrefImm, opStackrevrefImm, opImm, // 2
	opMakenry,
	opEntry,       opRef, opEntryKeep, opRefKeep, // 3
	opLength,      opOffset,         opSizeof, // 4
	opSwap,        opComma, // 5
	opNry,         opNrs,             opBrk // 6
};

char operationPrimitiveSymbols[] = {
	'$', '@', '!', // 1
	']', '>', '*', // 2
	'^', '[', '<', // 3
	'l', 'o', 't', // 4
	'~', ',', 'b', // 5
	'\0'
};

enum EvolutionEnum {
	Symbol, Idea, Opcode
};

enum opPrimitiveSymbolEnum {
	opsy_SRef,   opsy_SrRef,     opsy_Imm, // 1
	opsy_Index,  opsy_RIndex,    opsy_Keep, // 2
	opsy_Raise,  opsy_IndexOpen, opsy_RIndexOpen, // 3
	opsy_Length, opsy_Offset,    opsy_Sizeof, // 4
	opsy_Swap,   opsy_Comma,     opsy_break, // 5
	opsy_UNKNOWN, opsy_ANY,
	opsy_Page, opsy_Datum,
	opsy_opid
};

enum opIdeaEnum {
	opid_SRef,  opid_SrRef,  opid_Imm, // 1
	opid_Index, opid_RIndex, opid_IndexK, opid_RIndexK, // 2
	opid_Flip, opid_FlipR,
	opid_Raise, opid_Length, opid_Offset, opid_Sizeof, // 3
	opid_Swap,  opid_Comma,  opid_CommaK, opid_break, // 4
	opid_Page,  opid_Datum, // 5
	opid_ANY, opid_DEL
};

char checkIdeacorrectnesslist[][9] = {
	"--dD--PP", "--dD--PP", "--pP--dD", // 1
	"pPdDpPdD", "pPdDpPdD", "pPdDpPdD", "pPdDpPdD", // 2
	"--dD--pp", "--pP--dd", "--pP--dd", "--dD--dd", // 3
	"aAbBbBaA", "----____", "----__--", "--------", // 4
	// 5
	"\0end"
};

typedef struct {
	unsigned short conv[2];
	unsigned short args[2];
	unsigned short result[2];
} Rule;

enum { first, second };

#define CONV(a, b) { {(a), (b)}, 
#define RULE(a, b, c, d) {(a), (b)}, {(c), (d)} }

#define SymbolToIdeaAmount 18
Rule SymbolToIdea[SymbolToIdeaAmount] = {
	CONV(Symbol, Idea) RULE(opsy_SRef, opsy_ANY, opid_SRef, opid_ANY)
,	CONV(Symbol, Idea) RULE(opsy_SrRef, opsy_ANY, opid_SrRef, opid_ANY)
,	CONV(Symbol, Idea) RULE(opsy_Imm, opsy_ANY, opid_Imm, opid_ANY)

,	CONV(Symbol, Idea) RULE(opsy_Index, opsy_Keep, opid_IndexK, opid_DEL)
,	CONV(Symbol, Idea) RULE(opsy_RIndex, opsy_Keep, opid_RIndexK, opid_DEL)
,	CONV(Symbol, Idea) RULE(opsy_Index, opsy_ANY, opid_Index, opid_ANY)

,	CONV(Symbol, Idea) RULE(opsy_IndexOpen, opsy_ANY, opid_Flip, opid_ANY)
,	CONV(Symbol, Idea) RULE(opsy_RIndexOpen, opsy_ANY, opid_FlipR, opid_ANY)

,	CONV(Symbol, Idea) RULE(opsy_Raise, opsy_ANY, opid_Raise, opid_ANY)
,	CONV(Symbol, Idea) RULE(opsy_Length, opsy_ANY, opid_Length, opid_ANY)
,	CONV(Symbol, Idea) RULE(opsy_Offset, opsy_ANY, opid_Offset, opid_ANY)
,	CONV(Symbol, Idea) RULE(opsy_Sizeof, opsy_ANY, opid_Sizeof, opid_ANY)

,	CONV(Symbol, Idea) RULE(opsy_Swap, opsy_ANY, opid_Swap, opid_ANY)
,	CONV(Symbol, Idea) RULE(opsy_Comma, opsy_Keep, opid_CommaK, opid_DEL)
,	CONV(Symbol, Idea) RULE(opsy_Comma, opsy_ANY, opid_Comma, opid_ANY)
,	CONV(Symbol, Idea) RULE(opsy_break, opsy_ANY, opid_break, opid_ANY)

,	CONV(Symbol, Idea) RULE(opsy_Page, opsy_ANY, opid_Page, opid_ANY)
,	CONV(Symbol, Idea) RULE(opsy_Datum, opsy_ANY, opid_Datum, opid_ANY)
};

/*

	operation symbol: opsy ->
	operation idea:   opid ->
	 -- complex conversion dance --
	 -- error checking here? --
	 -- figuring out known information --
	operation :       op

	symbols -> ideas -> reformatting ->
	errorchecking -> known info -> compiling

*/

/*
Rule rulelist[] = {
	{opStackref,
		{opt_SRef, opt_VAROP}}
,	{opStackrevref,
		{opt_SrRef, opt_VAROP}}

,	{opStackrefImm,
		{opt_SRef, opt_Imm}}
,	{opStackrevrefImm,
		{opt_SrRef, opt_Imm}}
,	{opImm,
		{opt_Imm, opt_VAROP}}

,	{opMakenry,
		{opt_Raise, opt_VAROP}}
,	{opEntry,
		{opt_Index, opt_VAROP}}
,	{opRef,
		{opt_RIndex, opt_VAROP}}
,	{opEntryKeep,
		{opt_Index, opt_Keep}}
,	{opRefKeep,
		{opt_RIndex, opt_Keep}}
};*/

char typeString[][4] = {
	"chr", "i8", "u8",
	"i16", "u16", "i32", "u32",
	"i64", "u64", "f32", "f64", "\0end"
};

enum compareEnum {
	CE = 1, CG = 2, CS = 4,
};

#endif

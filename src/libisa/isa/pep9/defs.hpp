#pragma once

#include "magic_enum.hpp"
namespace isa::pep9 {
enum class instruction_mnemonic {

	STOP, RET, RETTR,
	MOVSPA,  MOVFLGA, MOVAFLG, 

	NOTA, NOTX, NEGA, NEGX,
	ASLA, ASLX, ASRA, ASRX,
	ROLA, ROLX, RORA, RORX,

	BR, BRLE, BRLT, BREQ, BRNE, BRGE, BRGT, BRV, BRC,
	CALL,
	NOP0, NOP1, NOP,
	DECI, DECO, HEXO, STRO,
	ADDSP, SUBSP,

	ADDA, ADDX, SUBA, SUBX,
	ANDA, ANDX, ORA, ORX,

	CPWA, CPWX, CPBA, CPBX,
	LDWA, LDWX, LDBA, LDBX,
	STWA, STWX, STBA, STBX,
};

enum class Registers {
	A = 0,
	X = 1,
	SP = 2,
	PC = 3,
	IS = 4,
	OS = 5,
};

enum class addressing_mode {
	NONE = 0,
	I = 1,
	D = 2,
	N = 4,
	S = 8,
	SF = 16,
	X = 32,
	SX = 64,
	SFX = 128,
	ALL = 255,
	INVALID
};

enum class addressing_class {
	Invalid,
	U_none, //?
	R_none, //?
	A_ix, //?
	AAA_all, //?
	AAA_i, //?
	RAAA_all, //?
	RAAA_noi

};
enum class memory_vectors {
	SYSTEM_STACK,
	TRAP,
};

enum class CSR {
	N,
	Z,
	V,
	C,
};
};

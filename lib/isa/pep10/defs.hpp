#pragma once

#include "magic_enum.hpp"
namespace isa::pep10 {
enum class instruction_mnemonic {

	RET, SRET,
	MOVSPA, MOVASP, MOVFLGA, MOVAFLG, MOVTA,
	USCALL,
	NOP,
	
	// FAULTS
	UNIMPL,

	NOTA, NOTX, NEGA, NEGX,
	ASLA, ASLX, ASRA, ASRX,
	ROLA, ROLX, RORA, RORX,
	//STOP,
	BR, BRLE, BRLT, BREQ, BRNE, BRGE, BRGT, BRV, BRC,
	CALL, SCALL,
	LDWT, LDWA, LDWX, LDBA, LDBX,
	STWA, STWX, STBA, STBX,
	CPWA, CPWX, CPBA, CPBX,
	ADDA, ADDX, SUBA, SUBX,
	ANDA, ANDX, ORA, ORX, XORA, XORX,
	ADDSP, SUBSP,
	
	MAX
};

enum class Registers {
	A = 0,
	X = 2,
	SP = 3,
	PC = 4,
	IS = 5,
	OS = 6,
	TR = 7
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
	MAX = 4
};
};
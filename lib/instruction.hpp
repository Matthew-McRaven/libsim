#pragma once

#include <array>
#include <string>
#include <vector>
#include <limits>
#include <tuple>

#include "magic_enum.hpp"
enum class instruction_mnemonic {
	#   define X(a) a,
	#   undef X
	RET, SRET,
	MOVSPA, MOVASP, MOVFLGA, MOVAFLG, MOVTA,
	NOP,
	USCALL,
	
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

enum class CSR {
	N,
	Z,
	V,
	C,
	MAX = 4
};

template <typename instr_width>
struct instruction_definition {
	instr_width bit_pattern = 0;
	addressing_class iformat = addressing_class::Invalid;
	std::array<bool, int(CSR::MAX)> CSR_modified = {false}; // Flag which CSR bits are changed by this instruction
	instruction_mnemonic mnemonic = instruction_mnemonic::MAX;
	std::string comment = {};
};

struct isa_definition {
	const std::array<instruction_definition<uint8_t>, (int) instruction_mnemonic::MAX> isa ;
	const std::array<std::tuple<const instruction_definition<uint8_t>*, addressing_mode>, 256> riproll;
	isa_definition();
};

const isa_definition isa;

std::string as_string(instruction_mnemonic);
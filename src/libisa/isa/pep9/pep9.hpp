#pragma once

#include <array>
#include <string>
#include <vector>
#include <limits>
#include <tuple>
#include <map>
#include <memory>

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

struct instruction_definition {
	uint8_t bit_pattern = 0;
	addressing_class iformat = addressing_class::Invalid;
	std::array<bool, magic_enum::enum_count<CSR>()> CSR_modified = {false}; // Flag which CSR bits are changed by this instruction
	instruction_mnemonic mnemonic = instruction_mnemonic::STOP;
	bool is_unary = false;
	std::string comment = {};
};

struct addr_map
{
	std::shared_ptr<instruction_definition> inst;
	addressing_mode addr;
};

struct isa_definition {
	const std::map<instruction_mnemonic, std::shared_ptr<instruction_definition>> isa ;
	const std::array<addr_map, 256> riproll;
	isa_definition();
	// Returns a static copy of a pep/9 isa definition.
	// Required for static initialization in a static library.
	static const isa_definition&  get_definition();
};

std::string as_string(instruction_mnemonic);
bool is_mnemonic_unary(instruction_mnemonic);
bool is_mnemonic_unary(uint8_t);
bool is_opcode_unary(instruction_mnemonic);
bool is_opcode_unary(uint8_t);
bool is_store(instruction_mnemonic);
bool is_store(uint8_t);

};

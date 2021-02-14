#pragma once

#include <array>
#include <string>
#include <vector>
#include <limits>
#include <tuple>
#include <map>
#include <memory>

#include "isa/pep10/defs.hpp"

namespace isa::pep10 {
template <typename instr_width>
struct instruction_definition {
	instr_width bit_pattern = 0;
	addressing_class iformat = addressing_class::Invalid;
	std::array<bool, magic_enum::enum_count<CSR>()> CSR_modified = {false}; // Flag which CSR bits are changed by this instruction
	instruction_mnemonic mnemonic = instruction_mnemonic::RET;
	bool is_unary = false;
	std::string comment = {};

};
struct addr_map
{
	std::shared_ptr<instruction_definition<uint8_t>> inst;
	addressing_mode addr;
};

struct isa_definition {
	const std::map<instruction_mnemonic, std::shared_ptr<instruction_definition<uint8_t>> > isa ;
	const std::array<addr_map, 256> riproll;
	// TODO: Make this constructor private since we have a singleton.
	isa_definition();
	// Returns a static copy of a pep/10 isa definition.
	// Required for static initialization in a static library.
	static const isa_definition&  get_definition();
};

std::string as_string(instruction_mnemonic);
bool is_opcode_unary(instruction_mnemonic);
bool is_opcode_unary(uint8_t);
bool is_store(instruction_mnemonic);
bool is_store(uint8_t);

};
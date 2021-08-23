#pragma once

#include <cstdint>
#include <vector>

#include <outcome.hpp>

#include "outcome_helper.hpp"
namespace masm::elf::mmio
{
enum class Type : uint16_t
{
	// 0 is invalid, so that a 0-init'ed memory won't act as a valid definition.
	kInput = 1,
	kOutput = 2
};

struct Definition
{
	Type type;
	// Indexes in the os.symtab symbol table which contains the name of this MMIO port.
	uint16_t symbol_table_offset;
};

std::vector<uint8_t> to_bytes(const std::vector<Definition>& definitions);
// WARNING!! This will probably be wrong on when moving between systems with different endianness.
// However, I don't forsee this as a feature worth having at this point.
result<std::vector<Definition>> from_bytes(const std::vector<uint8_t>& bytes);

}; // End namespace masm::elf::mmio
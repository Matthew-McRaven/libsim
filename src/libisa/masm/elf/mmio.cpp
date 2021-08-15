#include <array>

#include "mmio.hpp"
#include "io_error.hpp"

std::vector<uint8_t> masm::elf::mmio::to_bytes(const std::vector<masm::elf::mmio::Definition>& definitions)
{
	std::vector<uint8_t> ret_val;
	std::array<uint8_t, 4> arr;
	for(const auto& def : definitions){
		arr[0] = (static_cast<uint8_t>(def.type)>>8) & 0xff;
		arr[1] = static_cast<uint8_t>(def.type) & 0xff;
		arr[2] = (static_cast<uint8_t>(def.symbol_table_offset)>>8) & 0xff;
		arr[3] = static_cast<uint8_t>(def.symbol_table_offset) & 0xff;
		ret_val.insert(ret_val.end(), arr.cbegin(), arr.cend());
	}
	return ret_val;
}

result<std::vector<masm::elf::mmio::Definition>> masm::elf::mmio::from_bytes(const std::vector<uint8_t>& bytes)
{
	using Type = masm::elf::mmio::Type;
	if(bytes.size() % 4 != 0) return status_code(ElfMMIOErrc::BadByteCount);
	uint16_t type, index;
	std::vector<Definition> ret_val;
	for(size_t it = 0; it< bytes.size(); it += 4)
	{
		type = bytes[it] << 8 | bytes[it+1];
		auto type_enum = static_cast<Type>(type);
		// Enforce that our int->enum yields a valid value.
		if(type_enum != Type::kInput && type_enum != Type::kOutput) return status_code(ElfMMIOErrc::BadIOType);
		index = bytes[it+2] << 8 | bytes[it+3];
		// 0 is a special index that refers to nothing. Therefore, can't be valid symbol declaration.
		// Don't know how many total symbols there are, so we can't do any other checks here.
		if(index == 0) return status_code(ElfMMIOErrc::NoSuchIndex);
		ret_val.emplace_back(Definition{type_enum, index});
	}
	return ret_val;
}
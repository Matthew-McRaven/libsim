#include "mem_map.hpp"

#include <elfio/elfio.hpp>

#include "components/storage/base.hpp"
#include "components/storage/block.hpp"
#include "components/storage/layered.hpp"
#include "components/storage/mmio.hpp"
#include "components/storage/read_only.hpp"
#include "elf_tools/elf_helper.hpp"
#include "elf_tools/pep_elf_error.hpp"


template <bool enable_history>
result<std::tuple<uint16_t, std::shared_ptr<components::storage::Base<uint16_t, enable_history, uint8_t>>>>
	elf_tools::pep10::construct_os_storage(const ELFIO::elfio& image)
{
	auto os = elf_tools::find_section(image, "os.text");
	if(os == nullptr) return status_code(PepElfErrc::NoOSText);
	// Must convert size (1-indexed) to maximum offset (0-indexed), so must subtract 1.
	auto os_ram = std::make_shared<components::storage::Block<uint16_t, enable_history, uint8_t>>(os->get_size()-1);
	auto os_rom = std::make_shared<components::storage::ReadOnly<uint16_t, enable_history, uint8_t>>(os_ram, components::storage::WriteAttemptPolicy::kIgnore);
	return {os->get_address(), os_rom};
}

template <bool enable_history>
result<void> elf_tools::pep10::load_os_contents(const ELFIO::elfio& image, 
	std::shared_ptr<components::storage::Base<uint16_t, enable_history, uint8_t>> storage)
{
	auto os = elf_tools::find_section(image, "os.text");
	if(os == nullptr) return status_code(PepElfErrc::NoOSText);
	auto bytes = std::vector<uint8_t> {os->get_data(), os->get_data()+os->get_size()};
	components::storage::load_bytes<uint16_t, enable_history, uint8_t>(storage, bytes, 0);
	return result<void>(OUTCOME_V2_NAMESPACE::in_place_type<void>);

}

result<std::vector<elf_tools::pep10::PortDefinition>> elf_tools::pep10::port_definitions(const ELFIO::elfio& image)
{
	// Fetch symtab so we can resolve masm::elf::mmio::Definition::symbol_table_offset to a string.
	auto symtab = elf_tools::find_section(image, "os.symtab");
	if(symtab == nullptr) return status_code(PepElfErrc::NoOSSymtab);
	const ELFIO::symbol_section_accessor symbols(image, symtab);

	// Fetch mmio section, and run it through our custom decoder to get a vector of masm::elf::mmio::Definition.
	auto mmio = elf_tools::find_section(image, "os.mmio");
	if(mmio == nullptr) return status_code(PepElfErrc::NoOSMMIO);
	auto bytes = std::vector<uint8_t> {mmio->get_data(), mmio->get_data()+mmio->get_size()};
	// Will fail if bytes%4 != 0.
	auto ports_result = masm::elf::mmio::from_bytes(bytes);
	if(ports_result.has_error()) return ports_result.error().clone();
	auto ports = ports_result.value();

	// Don't pass size to ctor, or there will be a bunch of blank entires in vector.
	auto ret_val = std::vector<elf_tools::pep10::PortDefinition>();
	ret_val.reserve(ports.size());

	// Fields needed to perform ELFIO symbol lookup.
	// I will ignore most of them, but the function call will fail to compile without them.
	std::string name;
	ELFIO::Elf64_Addr value; ELFIO::Elf_Xword size;
	unsigned char bind, type, other;
	ELFIO::Elf_Half section_index;
	
	for(auto port : ports) {
		// It is UB if I attempt to index beyond the range of the symtab, therefore perform a length check.
		// offset is 0-index, symbols_num is 1-indexed.
		if (port.symbol_table_offset > symbols.get_symbols_num()-1) return status_code(PepElfErrc::BadMMIOSymbol);
		symbols.get_symbol(port.symbol_table_offset, name, value, size, bind, type, section_index, other);
		ret_val.emplace_back(elf_tools::pep10::PortDefinition{static_cast<uint16_t>(value), port.type, name});
	}
	return ret_val;
}

template <bool enable_history>
result<std::tuple<uint16_t, std::shared_ptr<components::storage::Base<uint16_t, enable_history, uint8_t>>>>
	elf_tools::pep10::construct_ram(const ELFIO::elfio& image)
{
	auto os = elf_tools::find_section(image, "os.text");
	if(os == nullptr) return status_code(PepElfErrc::NoOSText);
	// RAM will span from [0, start of os). Layered storage will prefer OS to RAM, however.
	// Must convert size (1-indexed) to maximum offset (0-indexed), so must subtract 1.
	auto ram = std::make_shared<components::storage::Block<uint16_t, enable_history, uint8_t>>(os->get_address()-1);
	return {0, ram};
}

// Create all memory-mapped ports for ELF definition.
template <bool enable_history>
result<std::vector<std::tuple<uint16_t, std::shared_ptr<components::storage::Base<uint16_t, enable_history, uint8_t>>>>>
	elf_tools::pep10::construct_mmio_ports(const ELFIO::elfio& image)
{
	auto ports = port_definitions(image);
	if(ports.has_error()) return ports.error().clone();
	auto ret = std::vector<std::tuple<uint16_t, std::shared_ptr<components::storage::Base<uint16_t, enable_history, uint8_t>>>>({});
	for(auto port : ports.value()) {
		std::shared_ptr<components::storage::Base<uint16_t, enable_history, uint8_t>> storage;
		if(port.type == masm::elf::mmio::Type::kInput)
			storage = std::make_shared<components::storage::Input<uint16_t, enable_history, uint8_t>>(0);
		else if(port.type == masm::elf::mmio::Type::kOutput)
			storage = std::make_shared<components::storage::Output<uint16_t, enable_history, uint8_t>>(0);
		else return status_code(PepElfErrc::InvalidMMIOType);
		ret.emplace_back(std::make_tuple(port.offset, storage));
	}
	return ret;
}

template <bool enable_history>
result<std::shared_ptr<components::storage::Layered<uint16_t, enable_history, uint8_t>>> 
	elf_tools::pep10::construct_memory_map(const ELFIO::elfio& image)
{
	// Create OS storage and load OS bytes.
	auto ROM_result = construct_os_storage<enable_history>(image);
	if(ROM_result.has_error()) ROM_result.error().clone();
	auto [ROM_offset, ROM_storage] = ROM_result.value();
	load_os_contents(image, ROM_storage);

	// Create RAM storage and DO NOT load user bytes.
	auto RAM_result = construct_ram<enable_history>(image);
	if(RAM_result.has_error()) RAM_result.error().clone();
	auto [RAM_offset, RAM_storage] = RAM_result.value();

	// Construct MMIO ports
	auto ports =  construct_mmio_ports<enable_history>(image);

	//Construct memory map for components
	using layered_t = components::storage::Layered<uint16_t, enable_history, uint8_t>;
	auto layered = std::make_shared<components::storage::Layered<uint16_t, enable_history, uint8_t>>(0xFFFF, 0, 
		layered_t::ReadMiss::kOOB, layered_t::WriteMiss::kOOB);
	for(auto [port_offset, port_storage] : ports.value()) layered->append_storage(port_offset, port_storage); 
	layered->append_storage(ROM_offset, ROM_storage);
	layered->append_storage(RAM_offset, RAM_storage);
	return layered;
}
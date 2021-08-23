#include "local_machine.hpp"


// Run the machine forever, or until a value is written to the pwrOff port, or a maximum number of steps is exceeded.
template<bool enable_history>
result<std::shared_ptr<isa::pep10::LocalMachine<enable_history>>> isa::pep10::machine_from_elf(const ELFIO::elfio& image)
{
	auto storage = elf_tools::pep10::construct_memory_map<enable_history>(image);
	if(storage.has_error()) return storage.error().clone();
	auto machine = std::make_shared<isa::pep10::LocalMachine<enable_history>>(storage.value());
	
	// Register MMIO ports with the machine.
	auto ports = elf_tools::pep10::port_definitions(image);
	if(ports.has_error()) return ports.error().clone();
	for(auto port : ports.value()) {
		machine->register_MMIO_address(port.name, port.offset);
	}

	return machine;
}
#include <outcome.hpp>

#include "local_machine.hpp"

using MPI = components::machine::MachineProcessorInterface<uint16_t, uint8_t, isa::pep10::memory_vectors>;
template<bool enable_history>
isa::pep10::LocalMachine<enable_history>::LocalMachine(
	std::shared_ptr<components::storage::Base<uint16_t, enable_history, uint8_t>> memory): 
	components::machine::MachineProcessorInterface<uint16_t, uint8_t, isa::pep10::memory_vectors>(),
	_memory(memory), 
	_processor(std::make_shared<isa::pep10::LocalProcessor>(
		static_cast<MPI&>(*this)))
{
	
}

template<bool enable_history>
result<bool> isa::pep10::LocalMachine<enable_history>::step()
{
	throw std::logic_error("Not implemented");
}

template<bool enable_history>
result<uint8_t> isa::pep10::LocalMachine<enable_history>::get_memory(uint16_t address) const
{
	throw std::logic_error("Not implemented");
}

template<bool enable_history>
result<void> isa::pep10::LocalMachine<enable_history>::set_memory(uint16_t address, uint8_t value)
{
	throw std::logic_error("Not implemented");
}

template<bool enable_history>
result<uint8_t> isa::pep10::LocalMachine<enable_history>::read_memory(uint16_t address) const
{
	throw std::logic_error("Not implemented");
}

template<bool enable_history>
result<void> isa::pep10::LocalMachine<enable_history>::write_memory(uint16_t address, uint8_t value)
{
	throw std::logic_error("Not implemented");
}

template<bool enable_history>
uint16_t isa::pep10::LocalMachine<enable_history>::read_register(isa::pep10::Register reg) const
{
	return isa::pep10::read_register(*_processor.get(), reg);
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::write_register(isa::pep10::Register reg, uint16_t value)
{
	return isa::pep10::write_register(*_processor.get(), reg, value);
}

template<bool enable_history>
bool isa::pep10::LocalMachine<enable_history>::read_csr(isa::pep10::CSR csr) const
{
	return isa::pep10::read_NZVC(*_processor.get(), csr);
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::write_csr(isa::pep10::CSR csr, bool value)
{
	return isa::pep10::write_NZVC(*_processor.get(), csr, value);
}

template<bool enable_history>
result<void> isa::pep10::LocalMachine<enable_history>::unwind_active_instruction()
{
	throw std::logic_error("Not implemented");
}

template<bool enable_history>
uint16_t isa::pep10::LocalMachine<enable_history>::address_from_vector(isa::pep10::memory_vectors vector) const
{
	throw std::logic_error("Not implemented");
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::begin_transaction()
{
	throw std::logic_error("Not implemented");
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::end_transaction()
{
	throw std::logic_error("Not implemented");
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::begin_instruction()
{
	throw std::logic_error("Not implemented");
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::end_instruction()
{
	throw std::logic_error("Not implemented");
}
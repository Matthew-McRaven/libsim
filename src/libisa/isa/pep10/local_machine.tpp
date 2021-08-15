#include <outcome.hpp>

#include <boost/range/adaptor/indexed.hpp>

#include "local_machine.hpp"

using MPI = components::machine::MachineProcessorInterface<uint16_t, uint8_t, isa::pep10::MemoryVector>;
template<bool enable_history>
isa::pep10::LocalMachine<enable_history>::LocalMachine(
	std::shared_ptr<components::storage::Base<uint16_t, enable_history, uint8_t>> memory): 
	components::machine::MachineProcessorInterface<uint16_t, uint8_t, isa::pep10::MemoryVector>(),
	_memory(memory), 
	_processor(std::make_shared<isa::pep10::LocalProcessor>(
		static_cast<MPI&>(*this)))
{
	
}

template<bool enable_history>
result<bool> isa::pep10::LocalMachine<enable_history>::step()
{
	
	auto ret = _processor->step();
	return ret;
}

template<bool enable_history>
bool isa::pep10::LocalMachine<enable_history>::halted() const
{
	throw std::logic_error("Not implemented.");
}

template<bool enable_history>
result<uint8_t> isa::pep10::LocalMachine<enable_history>::get_memory(uint16_t address) const
{
	return _memory->get(address);
}

template<bool enable_history>
result<void> isa::pep10::LocalMachine<enable_history>::set_memory(uint16_t address, uint8_t value)
{
	return _memory->set(address, value);
}

template<bool enable_history>
result<uint8_t> isa::pep10::LocalMachine<enable_history>::read_memory(uint16_t address) const
{
	return _memory->read(address);
}

template<bool enable_history>
result<void> isa::pep10::LocalMachine<enable_history>::write_memory(uint16_t address, uint8_t value)
{
	return _memory->write(address, value);
}

template<bool enable_history>
uint16_t isa::pep10::LocalMachine<enable_history>::max_offset() const
{
	return _memory->max_offset();
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
uint8_t isa::pep10::LocalMachine<enable_history>::read_packed_csr() const
{
	return isa::pep10::read_packed_NZVC(*_processor.get());
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::write_packed_csr(uint8_t value)
{
	return isa::pep10::write_packed_NZVC(*_processor.get(), value);
}

template<bool enable_history>
result<void> isa::pep10::LocalMachine<enable_history>::unwind_active_instruction()
{
	if constexpr(enable_history) {
		
		if(auto result_mem = _memory->take_delta(); result_mem.has_error()) return result_mem.error().clone();
		else result_mem.value()->apply_backward();

		if(auto result_reg = _processor->take_register_delta(); result_reg.has_error()) return result_reg.error().clone();
		else result_reg.value()->apply_backward();

		if(auto result_csr = _processor->take_csr_delta(); result_csr.has_error()) return result_csr.error().clone();
		else result_csr.value()->apply_backward();
		
		return result<void>(OUTCOME_V2_NAMESPACE::in_place_type<void>);
	}
	else {
		throw status_code(StorageErrc::DeltaDisabled);
	}
	

}

template<bool enable_history>
uint16_t isa::pep10::LocalMachine<enable_history>::address_from_vector(isa::pep10::MemoryVector vector) const
{
	uint16_t base = _memory->max_offset();
	switch(vector)
	{
		case isa::pep10::MemoryVector::kUser_Stack: return base - 11;
		case isa::pep10::MemoryVector::kSystem_Stack: return base - 9;
		case isa::pep10::MemoryVector::kPower_Off_Port: return base - 7;
		case isa::pep10::MemoryVector::kDispatcher: return base - 5;
		case isa::pep10::MemoryVector::kLoader: return base - 3;
		case isa::pep10::MemoryVector::kTrap_Handler: return base - 1;
	}
	throw std::logic_error("Invalid memory vector value.");
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::clear_all(uint8_t mem_fill, uint16_t reg_fill, bool csr_fill)
{
	clear_memory(mem_fill);
	clear_processor(reg_fill, csr_fill);
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::clear_memory(uint8_t mem_fill)
{
	_memory->clear(mem_fill);
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::clear_processor(uint16_t reg_fill, bool csr_fill)
{
	_processor->clear(reg_fill, csr_fill);
}


template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::begin_transaction()
{
	// TODO: Inform memory of transaction start.
	// Only necessary for cache model.
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::end_transaction()
{
	// TODO: Inform memory of transaction end.
	// Only necessary for cache model.
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::begin_instruction()
{
	// TODO: Pre-fetch the next instruction, and add statstic to histogram.
}

template<bool enable_history>
void isa::pep10::LocalMachine<enable_history>::end_instruction()
{
	// TODO: Push all deltas onto the history stack.
}

template<bool enable_history>
result<void> isa::pep10::load_bytes(std::shared_ptr<isa::pep10::LocalMachine<enable_history>> machine, 
	std::vector<uint8_t> bytes, uint16_t offset)
{
	for(auto [index, byte] : bytes | boost::adaptors::indexed(0))
	{
		auto res = machine->write_memory((offset+index)%machine->max_offset(), byte);
		if(res.has_failure()) return res.error().clone();
	}
	return result<void>(OUTCOME_V2_NAMESPACE::in_place_type<void>);
}
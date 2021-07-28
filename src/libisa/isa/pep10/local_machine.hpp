#pragma once

#include <outcome.hpp>

#include "components/machine/interace.hpp"
#include "components/storage/base.hpp"
#include "isa/pep10/local_processor.hpp"
#include "isa/pep10/pep10.hpp"


namespace isa::pep10 {

template <bool enable_history>
class LocalMachine : public components::machine::MachineProcessorInterface<uint16_t, uint8_t, isa::pep10::memory_vectors> 
{
public:
	// C.67 suppress polymorphic copy/move.
	// Construct processor in place from this.
	LocalMachine(std::shared_ptr<components::storage::Base<uint16_t, enable_history, uint8_t>> memory);
	~LocalMachine() override = default;

	/*
	 * Accessors / modifiers for processor.
	 */
	result<bool> step();
	/*
	 * Implement MachineProcessorInterface.
	 */
	bool halted() const override;
	result<uint8_t> get_memory(uint16_t address) const override;
	result<void> set_memory(uint16_t address, uint8_t value) override;
	result<uint8_t> read_memory(uint16_t address) const override;
	result<void> write_memory(uint16_t address, uint8_t value) override;
	uint16_t max_offset() const override;
	// Don't use results here. Failing a register read/write is a fatal error that will crash the progam.
	uint16_t read_register(isa::pep10::Register reg) const;
	void write_register(isa::pep10::Register reg, uint16_t value);
	bool read_csr(isa::pep10::CSR csr) const;
	void write_csr(isa::pep10::CSR csr, bool value);

	result<void> unwind_active_instruction() override;
	uint16_t address_from_vector(isa::pep10::memory_vectors vector) const override;
private:

	std::shared_ptr<components::storage::Base<uint16_t, enable_history, uint8_t>> _memory;
	std::shared_ptr<isa::pep10::LocalProcessor> _processor;

	/*
	 * Implement MachineProcessorInterface.
	 */
	void begin_transaction() override;
	void end_transaction() override;
	void begin_instruction() override;
	void end_instruction() override;
};

} // namespace isa::pep10
#include "local_machine.tpp"
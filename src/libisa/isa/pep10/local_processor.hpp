#pragma once

#include <stdint.h>

#include "components/machine/processor_model.hpp"
#include "components/machine/interace.hpp"
#include "components/storage/block.hpp"
#include "isa/pep10/pep10.hpp"
namespace isa::pep10 {

class LocalProcessor : public components::machine::ProcessorModel<uint16_t, uint8_t, uint16_t, uint8_t, bool, true> 
{
public:
	LocalProcessor(components::machine::MachineProcessorInterface<uint16_t, uint8_t, isa::pep10::MemoryVector>& owner);
	

	// Interface that must be implemented by deriving processor models
	result<bool> step() override;
	bool can_step_into() const override;
	uint16_t call_depth() const override;

	// Set up / tear down
	void init() override;
	void debug(bool) override;
	void clear(uint16_t reg_fill, bool csr_fill) override;

	// Read / write registers
	uint16_t read_register(uint8_t reg_number) const override;
	void write_register(uint8_t reg_number, uint16_t value) override;
	uint8_t register_count() const override;

	bool read_csr(uint8_t csr_number) const override;
	void write_csr(uint8_t csr_number, bool value) override;
	uint8_t csr_count() const override;

	// Statistics
	uint64_t cycle_count() const override;
	uint64_t instruction_count() const override;

	// Todo: How do I coallate deltas for the CPU register bank
	void* take_delta() override;
	
private:
	components::machine::MachineProcessorInterface<uint16_t, uint8_t, isa::pep10::MemoryVector>& _owner;
	std::shared_ptr<components::storage::Block<uint8_t, true, uint16_t>> _registers {nullptr};
	std::shared_ptr<components::storage::Block<uint8_t, true, bool>> _csrs {nullptr};
	uint64_t _cycle_count {0};

	result<void> unary_dispatch(uint8_t is);
	result<void> nonunary_dispatch(uint8_t is, uint16_t os);

	result<uint8_t> read_byte(uint16_t address) const;
	result<uint16_t> read_word(uint16_t address) const;
	result<void> write_byte(uint16_t address, uint8_t value);
	result<void> write_word(uint16_t address, uint16_t value);

	result<uint16_t> decode_load_operand(const isa::pep10::instruction_definition& is, isa::pep10::addressing_mode mode, uint16_t addr);
	result<uint16_t> decode_store_operand(const isa::pep10::instruction_definition& is, isa::pep10::addressing_mode mode, uint16_t addr);
};

uint16_t read_register(const LocalProcessor& proc, isa::pep10::Register reg);
void write_register(LocalProcessor& proc, isa::pep10::Register reg, uint16_t value);

bool read_NZVC(const LocalProcessor&proc, isa::pep10::CSR reg);
void write_NZVC(LocalProcessor& proc, isa::pep10::CSR reg, bool value);
uint8_t read_packed_NZVC(const LocalProcessor& proc);
void write_packed_NZVC(LocalProcessor& proc, uint8_t packed);

};
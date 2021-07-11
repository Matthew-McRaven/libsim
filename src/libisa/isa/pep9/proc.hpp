#include <stdint.h>

#include "isa/env_interface.hpp"
#include "isa/pep9/instruction.hpp"
#include "isa/pep9/defs.hpp"
namespace isa::pep9 {

class isa_processor {
public:
	isa_processor(isa::env_interface<uint16_t, uint16_t, bool> iface);
	int step();
	bool can_step_into() const;
	void init();
	void debug(bool);
	void clear();

	using Registers = isa::pep9::Registers;
	using CSR = isa::pep9::CSR;
	using reg_size_t = uint16_t;
	using csr_size_t = bool;
private:
	uint16_t addr_from_vector(isa::pep9::memory_vectors vector);
	isa::env_interface<uint16_t, uint16_t, bool> _iface;
	void unary_dispatch(uint8_t is);
	void nonunary_dispatch(uint8_t is, uint16_t os);
	uint16_t decode_load_operand(const instruction_definition<uint8_t>& is, addressing_mode mode, uint16_t addr) const;
	uint16_t decode_store_operand(const instruction_definition<uint8_t>& is, addressing_mode mode, uint16_t addr) const;

	uint16_t read_reg(Registers reg) const;
	void write_reg(Registers reg, uint16_t value);
	uint8_t read_byte(uint16_t address, bool) const;
	uint16_t read_word(uint16_t address, bool) const;
	void write_byte(uint16_t address, uint8_t value, bool) const;
	void write_word(uint16_t address, uint16_t value, bool);
	
	void set_NZVC(uint8_t packed);
	void set_NZVC(CSR reg, bool value);
	uint8_t get_packed_NZVC() const;
	bool get_NZVC(CSR reg) const;
};
};
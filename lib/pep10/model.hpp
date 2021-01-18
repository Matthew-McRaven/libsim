#include <stdint.h>

#include "interface.hpp"
#include "pep10/instruction.hpp"
namespace pep10::isa {

class isa_processor {
public:
	isa_processor(pep10::isa::interface<uint16_t, uint16_t, bool> iface);
	int step();
	void init();
	void debug(bool);
	void clear();
private:
	pep10::isa::interface<uint16_t, uint16_t, bool> _iface;
	void unary_dispatch(uint8_t is);
	void nonunary_dispatch(uint8_t is, uint16_t os);
	uint16_t decode_load_operand(const instruction_definition<uint8_t>* is, addressing_mode mode, uint16_t addr) const;
	uint16_t decode_store_operand(const instruction_definition<uint8_t>* is, addressing_mode mode, uint16_t addr) const;

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
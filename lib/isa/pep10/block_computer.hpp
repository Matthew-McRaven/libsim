#pragma once
#include "isa/step_delta.hpp"

#include "isa/pep10/defs.hpp"
#include "isa/pep10/instruction.hpp"
#include "isa/pep10/proc.hpp"
namespace isa::pep10
{
class block_computer
{

	public:
	isa::delta_timestep<isa::pep10::Registers, uint16_t, 
		isa::pep10::CSR, bool, uint16_t> magic;
	using delta_t = isa::delta_timestep<isa::pep10::Registers, uint16_t, 
		isa::pep10::CSR, bool, uint16_t>;
	block_computer();
	delta_t step();
	delta_t apply(delta_t);
	delta_t get_delta() const {return current_step;}

	public:
	// Implement proc_interface
	uint8_t read_mem_byte(uint16_t addr, bool disable_side_effects) const;
	void write_mem_byte(uint16_t addr, uint8_t value, bool disable_side_effects);
	void clear_memory();

	uint16_t read_reg(uint8_t) const;
	void write_reg(uint8_t, uint16_t);
	void clear_regs();

	uint16_t read_csr(uint8_t) const;
	void write_csr(uint8_t, bool);
	void clear_csrs();
	private:
	delta_t current_step;

	std::array<uint16_t, 8> regbank;
	std::array<uint8_t, 0x10000> memory;
	std::array<bool, 4> NZVC;
	isa::interface<uint16_t, uint16_t, bool> _iface;
	isa::pep10::isa_processor proc;

};
} // namespace pep10::isa

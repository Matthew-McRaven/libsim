#pragma once

#include <type_traits>

#include "isa/step_delta.hpp"
#include "isa/sim_interface.hpp"

#include "isa/pep10/defs.hpp"
#include "isa/pep10/instruction.hpp"
#include "isa/pep10/proc.hpp"
namespace isa::pep10
{

template <typename regbank_t, typename mem_device_t>
class block_computer
{

	public:
	using delta_t = isa::delta_timestep<uint8_t, uint16_t, 
		uint8_t, bool, uint16_t>;
	using sim_interface_t = isa::sim_interface<uint8_t, uint16_t, 
		uint8_t, bool, uint16_t>;
	block_computer(regbank_t regbank, mem_device_t memory);
	sim_interface_t sim_interface();

	delta_t step();

	//delta_t step();
	delta_t apply(delta_t);
	delta_t get_delta() const; 

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

	regbank_t regbank;
	mem_device_t memory;

	isa::env_interface<uint16_t, uint16_t, bool> _iface;
	isa::pep10::isa_processor proc;

};
#include "isa/pep10/block_computer.tpp"
} // namespace pep10::isa

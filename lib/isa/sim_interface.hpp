#pragma once
#include <stdint.h>
#include <functional>

#include "isa/step_delta.hpp"

namespace isa {
template < typename reg_name_t, typename reg_size_t, 
	typename csr_name_t, typename csr_size_t, 
	typename addr_size_t>
struct sim_interface {
	using delta_t= isa::delta_timestep<
		reg_name_t, reg_size_t,
		csr_name_t, csr_size_t, 
		addr_size_t>;

	std::function<uint8_t(addr_size_t, bool)> read_mem_byte;
	std::function<void(addr_size_t, uint8_t, bool)> write_mem_byte;
	std::function<void()> clear_mem;

	std::function<reg_size_t(uint8_t)> read_reg;
	std::function<void(uint8_t, reg_size_t)> write_reg;
	std::function<void()> clear_regs;

	std::function<csr_size_t(uint8_t)> read_csr;
	std::function<void(uint8_t, csr_size_t)> write_csr;
	std::function<void()> clear_csr;

	std::function<delta_t() > step;
	std::function<delta_t()> get_delta;
	std::function<delta_t(delta_t)> apply;

	std::function<void(addr_size_t)> add_code_breakpoint;
	std::function<void(addr_size_t)> remove_code_breakpoint;	
	std::function<void()> clear_code_breakpoints;
	
};
};
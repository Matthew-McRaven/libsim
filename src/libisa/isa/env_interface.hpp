#pragma once
#include <stdint.h>
#include <functional>

namespace isa {
template <typename addr_size_t, typename reg_size_t, typename csr_size_t>
struct env_interface {
	addr_size_t max_addr = {0xffff};
	
	std::function<void()> begin_instruction;
	std::function<void()> end_instruction;

	std::function<void()> begin_transaction;
	std::function<void()> end_transaction;
	std::function<uint8_t(addr_size_t, bool)> read_mem_byte;
	std::function<void(addr_size_t, uint8_t, bool)> write_mem_byte;
	std::function<void()> clear_mem;

	std::function<reg_size_t(uint8_t)> read_reg;
	std::function<void(uint8_t, reg_size_t)> write_reg;
	std::function<void()> clear_regs;

	std::function<csr_size_t(uint8_t)> read_csr;
	std::function<void(uint8_t, csr_size_t)> write_csr;
	std::function<void()> clear_csrs;
	
};
};
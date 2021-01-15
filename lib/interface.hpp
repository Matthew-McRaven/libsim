#pragma once
#include "defs.hpp"
template <typename addr_size_t, typename reg_size_t, typename CSR_size_t>
struct interface {
	void (*begin_instruction)();
	void (*end_instruction)();

	void (*begin_transaction)();
	void (*end_transaction)();
	uint8_t  (*read_mem_byte)(addr_size_t address, bool supress_io);
	void  (*write_mem_byte)(addr_size_t address, uint8_t value, bool supress_io);
	uint16_t (*addr_from_vector)(memory_vectors vec);
	void (*clear_mem)();

	reg_size_t (*read_reg)(uint8_t reg);
	void (*write_reg)(uint8_t reg, reg_size_t value);
	void (*clear_regbank)();

	CSR_size_t (*read_csr)(uint8_t csr);
	void (*write_csr)(uint8_t csr, CSR_size_t value);
	void (*clear_CSR)();	
};
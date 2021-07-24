#pragma once

#include <stdint.h>
#include <vector>

#include "components/storage/block.hpp"

// TODO: Migrate code to .TPP file.
namespace components::reg {
	template<typename reg_size_t, typename csr_size_t>
	class block_register 
	{
	public:
		block_register(uint8_t reg_count, uint8_t csr_count):
			regs(reg_count), csrs(csr_count)
		{}

		result<reg_size_t> read_reg(uint8_t num) const
		{
			return regs.read(num);
		}

		result<void> write_reg(uint8_t num, reg_size_t val)
		{
			return regs.write(num, val);
		}

		void clear_regs()
		{
			regs.clear();
		}

		result<csr_size_t> read_csr(uint8_t num) const
		{
			return csrs.read(num);
		} 

		result<void> write_csr(uint8_t num, csr_size_t val) 
		{
			return csrs.write(num, val);
		} 

		void clear_csrs()
		{
			csrs.clear();
		}
	private:
		components::storage::Block<uint8_t, reg_size_t> regs;
		components::storage::Block<uint8_t, csr_size_t> csrs;
	};
};
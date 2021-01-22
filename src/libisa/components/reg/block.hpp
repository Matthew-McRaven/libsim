#pragma once

#include <stdint.h>
#include <vector>

namespace components::reg {
	template<typename reg_size_t, typename csr_size_t>
	class block_register 
	{
	public:
		block_register(uint8_t reg_count, uint8_t csr_count):
			registers(reg_count), csrs(csr_count)
		{

		}
		reg_size_t read_reg(uint8_t num) const {return registers[num];} 
		void write_reg(uint8_t num, reg_size_t val) {registers[num]=val;}
		void clear_regs(){
			std::fill(registers.begin(), registers.end(), 0);
		}

		csr_size_t read_csr(uint8_t num) const {return csrs[num];} 
		void write_csr(uint8_t num, csr_size_t val) {csrs[num]=val;} 
		void clear_csrs(){
			std::fill(csrs.begin(), csrs.end(), 0);
		}
	private:
		std::vector<reg_size_t> registers;
		std::vector<csr_size_t> csrs;
	};
};
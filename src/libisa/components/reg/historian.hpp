#pragma once

#include <stdint.h>
#include <vector>
#include <utility>
#include <map>

namespace components::reg {
	template<typename regbank_t>
	class historian 
	{
	public:
		historian(regbank_t regbank):
			regbank(regbank)
		{

		}

		using reg_size_t = decltype(((regbank_t*)nullptr)->read_reg(0));
		using csr_size_t = decltype(((regbank_t*)nullptr)->read_csr(0));

		outcome<reg_size_t> read_reg(uint8_t num) const
		{
			return regbank.read_reg(num);
		} 

		outcome<void> write_reg(uint8_t num, reg_size_t val)
		{
			auto search =  delta_reg.find(num);
			if(search != delta_reg.end()) {
				search->second = {std::get<0>(search->second), val};
			}
			else {
				delta_reg[num] = {read_csr(num), val};
			}
			return regbank.write_reg(num, val);
		}

		void clear_regs()
		{
			delta_reg.clear();
			regbank.clear_regs();
		}

		std::map<uint8_t, std::tuple<reg_size_t, reg_size_t> > get_reg_delta() const
		{
			return delta_reg;
		}

		outcome<csr_size_t> read_csr(uint8_t num) const
		{
			return regbank.read_reg(num);
		} 

		outcome<void> write_csr(uint8_t num, csr_size_t val) 
		{
			auto search =  delta_csr.find(num);
			if(search != delta_csr.end()) {
				search->second = {std::get<0>(search->second), val};
			}
			else {
				delta_csr[num] = {read_csr(num), val};
			}
			return regbank.write_csr(num, val);
		}

		void clear_csrs()
		{
			delta_csr.clear();
			regbank.clear_csrs();
		}

		std::map<uint8_t, std::tuple<csr_size_t, csr_size_t> > get_csr_delta() const
		{
			return delta_csr;
		}
	private:
		regbank_t regbank;
		std::map<uint8_t, std::tuple<reg_size_t, reg_size_t> > delta_reg;
		std::map<uint8_t, std::tuple<csr_size_t, csr_size_t> > delta_csr;
	};
};
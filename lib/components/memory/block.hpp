#pragma once

#include <stdint.h>
#include <vector>

namespace components::memory {
	template<typename _addr_size_t>
	class block_memory 
	{
	public:
	using addr_size_t = _addr_size_t;
		block_memory(addr_size_t max_addr):
			memory(max_addr+1)
		{

		}
		uint8_t read_byte(addr_size_t num, bool) const {return memory[num];} 
		void write_byte(addr_size_t num, uint8_t val, bool) {memory[num]=val;}
		void clear(){
			std::fill(memory.begin(), memory.end(), 0);
		}
	private:
		std::vector<uint8_t> memory;
	};
};
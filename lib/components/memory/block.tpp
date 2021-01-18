#pragma once

#include <stdint.h>
#include <vector>

namespace components::memory {

	template <typename mem_device_t, typename addr_size_t>
	class block_memory 
	{
		block_memory(addr_size_t max_addr):memory(max_addr){};
		uint8_t read_byte(addr_size_t addr, bool disable_side_effects) const
		{
			return memory[addr];
		};
		void write_byte(addr_size_t addr, uint8_t value, bool disable_side_effects)
		{
			return memory[addr] = value;
		}
		void get_delta() const{};
		void on_begin_instruction(){};
		void on_end_instruction(){};
		void on_begin_transaction(){};
		void on_end_transaction(){};
	private:
		std::vector<uint8_t> memory;

	};
}
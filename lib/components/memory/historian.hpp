#pragma once

#include <stdint.h>
#include <vector>

namespace components::memory {
	template<typename addr_size_t>
	struct delta_memory
	{
		std::tuple<addr_size_t, addr_size_t> addr_span;
		std::vector<uint8_t> old_value, new_value;
	};
	template <typename mem_device_t, typename addr_size_t>
	class historian
	{
	public:
		using delta_mem_t = std::vector<delta_memory<addr_size_t> >;
		historian(mem_device_t device):device(device){};
		uint8_t read_byte(addr_size_t addr, bool disable_side_effects) const
		{
			return device.read_byte(addr, disable_side_effects);
		};
		
		void write_byte(addr_size_t addr, uint8_t value, bool disable_side_effects)
		{
			bool inserted = false, check_merge=false;
			// Attempt to modify an existing delta in-place if possible.
			for(auto& delta : delta_mem) {
				if (auto dist = addr - std::get<0>(delta.addr_span);
					dist <= std::get<1>(delta.addr_span) - std::get<0>(delta.addr_span)) {
						delta.new_value[dist] = value;
						inserted = true;
				}
				// Only extend uperwards, depend on merg step to combine downward.
				else if(addr == std::get<1>(delta.addr_span) + 1) {
					delta.addr_span = {std::get<0>(delta.addr_span), addr};
					delta.new_value.push_back(value);
					delta.old_value.push_back(read_byte(addr, true));
					inserted = true;
					check_merge = true;
				}
			}
			// Couldn't find an exisiting delta to modify, so must create new delta.
			if(!inserted) {
				delta_mem.push_back({{addr, addr}, {read_byte(addr, true)},{value}});
				check_merge = true;
			}
			// Check if we can merge any two deltas into a single delta.
			// Since we apply this step anytime our data changes, there is at most one change,
			// which means ranges may now be connected, but no range will overlap (i.e. have 
			// duplicate data elements).
			if(check_merge) {
				
				using _t = delta_memory<uint16_t>;
				// Will compare on first key of tuple. Sufficient behavior because there should be no overlap between segments.
				auto compare = [](_t lhs, _t rhs){return lhs.addr_span<rhs.addr_span;};
				std::sort(delta_mem.begin(), delta_mem.end(), compare);

				// Allow endpoint to be re-computed as we remove elements
				for(int it=0; it<delta_mem.size()-1; it++) {
					// Since our deltas are sorted, only next could be adjacent to
					// current on current's right/upper bound. Since we iterate from 0>0xffff,
					// no need to compare LHS.
					auto cur = delta_mem[it];
					auto next = delta_mem[it+1];
					// Delta only borders in upperbound and lower bound are adjacent integers.
					if(std::get<1>(cur.addr_span)+1 == std::get<0>(next.addr_span)) {
						// Merge current+next into current.
						cur.addr_span = {std::get<0>(cur.addr_span), std::get<1>(next.addr_span)};
						cur.old_value.insert( cur.old_value.end(), next.old_value.begin(), next.old_value.end() );
						cur.new_value.insert( cur.new_value.end(), next.new_value.begin(), next.new_value.end() );
						// Remove next delta, since it has been merged in.
						delta_mem.erase(delta_mem.begin()+it+1);
					}
				}


			}
			return device.write_byte(addr, value, disable_side_effects);
		}
		void clear() {
			delta_mem.clear();
			device.clear();
		}
		delta_mem_t get_delta() const{return delta_mem;};
		void clear_delta() {delta_mem.clear();}
		void on_begin_instruction(){};
		void on_end_instruction(){};
		void on_begin_transaction(){};
		void on_end_transaction(){};
	private:
		mem_device_t device;
		std::vector<delta_memory<addr_size_t> > delta_mem;

	};
}
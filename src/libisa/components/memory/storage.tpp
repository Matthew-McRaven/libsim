#include "storage.hpp"

//#include <format>
#include <fmt/core.h>
template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
[[noreturn]] void components::memory::oob_read_helper(offset_t offset)
{
	// TODO: Replace libfmt with STL's std::format. 
    throw std::out_of_range(fmt::format("Out of range memory read at: {x}", offset)); 
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
[[noreturn]] void components::memory::oob_write_helper(offset_t offset, uint8_t value)
{
	// TODO: Replace libfmt with STL's std::format. 
    throw std::out_of_range(fmt::format("Out of range memory write at: {x}", offset)); 
}

/*
* Block-based storage device.
*/
template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
components::memory::storage_block<offset_t>::storage_block(offset_t max_offset): _max_offset(max_offset),
	_memory(std::vector<uint8_t>(_max_offset + 1))
{

}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
void components::memory::storage_block<offset_t>::clear(uint8_t fill_val)
{
	std::fill(_memory.begin(), _memory.end(), fill_val);
}
template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
uint8_t components::memory::storage_block<offset_t>::read_byte(offset_t offset) const
{
	if(offset > _max_offset) oob_read_helper(offset);
	else return _memory.at(offset);
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
uint8_t components::memory::storage_block<offset_t>::get_byte(offset_t offset) const
{
	if(offset > _max_offset) oob_read_helper(offset);
	else return _memory.at(offset);
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
void components::memory::storage_block<offset_t>::write_byte(offset_t offset, uint8_t value)
{
	if(offset > _max_offset) oob_write_helper(offset, value);
	else _memory[offset] = value;
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
void components::memory::storage_block<offset_t>::set_byte(offset_t offset, uint8_t value)
{
	if(offset > _max_offset) oob_write_helper(offset, value);
	else _memory[offset] = value;
}


template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
offset_t components::memory::storage_block<offset_t>::max_offset() const noexcept
{
	return _max_offset;
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
void components::memory::storage_block<offset_t>::resize(offset_t new_offset) noexcept
{
	_max_offset = new_offset;
	_memory.resize(new_offset+1);
	clear();
}

/*
 * Map-based storage device.
 */
template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
components::memory::storage_map<offset_t>::storage_map(offset_t max_offset, uint8_t default_value): 
	_max_offset(max_offset), _default(default_value), _memory()
{

}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
void components::memory::storage_map<offset_t>::clear(uint8_t fill_val)
{
	_memory.clear();
	_default = fill_val;
}
template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
uint8_t components::memory::storage_map<offset_t>::read_byte(offset_t offset) const
{
	if(offset > _max_offset) oob_read_helper(offset);
	else if(auto key = _memory.find(offset); key != _memory.end()) return key->second;
	else return _default;
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
uint8_t components::memory::storage_map<offset_t>::get_byte(offset_t offset) const
{
	if(offset > _max_offset) oob_read_helper(offset);
	else if(auto key = _memory.find(offset); key != _memory.end()) return key->second;
	else return _default;
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
void components::memory::storage_map<offset_t>::write_byte(offset_t offset, uint8_t value)
{
	if(offset > _max_offset) oob_write_helper(offset, value);
	else _memory[offset] = value;
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
void components::memory::storage_map<offset_t>::set_byte(offset_t offset, uint8_t value)
{
	if(offset > _max_offset) oob_write_helper(offset, value);
	else _memory[offset] = value;
}


template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
offset_t components::memory::storage_map<offset_t>::max_offset() const noexcept
{
	return _max_offset;
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
void components::memory::storage_map<offset_t>::resize(offset_t new_offset) noexcept
{
	_max_offset = new_offset;
	clear();
}

/*
 * Range-based storage device.
 */
template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
components::memory::storage_range<offset_t>::storage_range(offset_t max_offset, uint8_t default_value): 
	_max_offset(max_offset), _default(default_value), _memory()
{

}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
void components::memory::storage_range<offset_t>::clear(uint8_t fill_val)
{
	_memory.clear();
	_default = fill_val;
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
uint8_t components::memory::storage_range<offset_t>::read_byte(offset_t offset) const
{
	static auto comp = [](const components::memory::memory_span<offset_t>& lhs, offset_t rhs){
		return std::get<0>(lhs.span) < rhs;
	};

	if(offset > _max_offset) oob_read_helper(offset);
	else {
		for(auto& span : _memory) {
			if (auto dist = offset - std::get<0>(span.span);
				dist <= std::get<1>(span.span) - std::get<0>(span.span)) {
					return span.value[dist];
			}
		}
	}
	return _default;
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
uint8_t components::memory::storage_range<offset_t>::get_byte(offset_t offset) const
{
	return read_byte(offset);
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
void components::memory::storage_range<offset_t>::write_byte(offset_t offset, uint8_t value)
{
	if(offset > _max_offset) oob_write_helper(offset, value);

	bool inserted = false, check_merge = false;
	// Attempt to modify an existing delta in-place if possible.
	for(auto& span : _memory) {
		if (auto dist = offset - std::get<0>(span.span);
			dist <= std::get<1>(span.span) - std::get<0>(span.span)) {
				span.value[dist] = value;
				inserted = true;
		}
		// Only extend uperwards, depend on merg step to combine downward.
		else if(offset == std::get<1>(span.span) + 1) {
			span.span = {std::get<0>(span.span), offset};
			span.value.push_back(value);
			inserted = true;
			check_merge = true;
		}
	}
	// Couldn't find an exisiting delta to modify, so must create new delta.
	if(!inserted) {
		_memory.push_back({{offset, offset}, {value}});
		check_merge = true;
	}
	// Check if we can merge any two deltas into a single delta.
	// Since we apply this step anytime our data changes, there is at most one change,
	// which means ranges may now be connected, but no range will overlap (i.e. have 
	// duplicate data elements).
	if(check_merge) {
		
		using _t = components::memory::memory_span<offset_t>;
		// Will compare on first key of tuple. Sufficient behavior because there should be no overlap between segments.
		auto compare = [](_t lhs, _t rhs){return lhs.span<rhs.span;};
		std::sort(_memory.begin(), _memory.end(), compare);

		// Allow endpoint to be re-computed as we remove elements
		for(int it=0; it<_memory.size()-1; it++) {
			// Since our deltas are sorted, only next could be adjacent to
			// current on current's right/upper bound. Since we iterate from 0>0xffff,
			// no need to compare LHS.
			auto cur = _memory[it];
			auto next = _memory[it+1];
			// Delta only borders in upperbound and lower bound are adjacent integers.
			if(std::get<1>(cur.span)+1 == std::get<0>(next.span)) {
				// Merge current+next into current.
				cur.span = {std::get<0>(cur.span), std::get<1>(next.span)};
				cur.value.insert(cur.value.end(), next.value.begin(), next.value.end());
				// Remove next delta, since it has been merged in.
				_memory.erase(_memory.begin()+it+1);
			}
		}
	}
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
void components::memory::storage_range<offset_t>::set_byte(offset_t offset, uint8_t value)
{
	write_byte(offset, value);
}


template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
offset_t components::memory::storage_range<offset_t>::max_offset() const noexcept
{
	return _max_offset;
}

template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
void components::memory::storage_range<offset_t>::resize(offset_t new_offset) noexcept
{
	_max_offset = new_offset;
	clear();
}
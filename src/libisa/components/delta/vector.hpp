#pragma once

#include "components/delta/base.hpp"
#include "components/storage/helper.hpp"

namespace components::delta 
{

template<typename offset_t, typename val_size_t=uint8_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
class Vector : public components::delta::Base<offset_t, val_size_t>
{
public:
	Vector(components::storage::Base<offset_t, true, val_size_t>& storage);
	~Vector() = default;
	void apply_backward() override;
	void apply_forward() override;
	void clear() override;
	void add_delta(offset_t offset, val_size_t old_value, val_size_t new_value);
private:
	components::storage::Base<offset_t, true, val_size_t>& _storage;
	// Store changes in a vector, sorted from smallest to largest offset.
	// Keep elements with duplicate offsets---skipping them would cause replay to differ from execution.
	std::vector<std::tuple<offset_t, val_size_t /*old*/, val_size_t/*new*/>> _deltas;
	
};
} // namespace components::delta

#include "vector.tpp"
#pragma once

#include "components/delta/base.hpp"
#include "components/storage/helper.hpp"

namespace components::delta 
{

template<typename offset_t, typename val_size_t=uint8_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
class Grouped : public components::delta::Base<offset_t, val_size_t>
{
public:
	using delta_t = std::unique_ptr<components::delta::Base<offset_t, val_size_t>>;
	// Does not correspond to a physical storage device, but rather a group of devices. 
	// Therefore, do not take a device as an argument. 
	Grouped();
	~Grouped() = default;
	void apply_backward() override;
	void apply_forward() override;
	void clear() override;
	void add_delta(delta_t delta);
private:
	// Store all layers' deltas in a vector.
	// Each delta applies to a different storage device.
	std::vector<delta_t> _deltas;
};
} // namespace components::delta

#include "grouped.tpp"
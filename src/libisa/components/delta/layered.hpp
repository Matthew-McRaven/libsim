#pragma once

#include <outcome.hpp>

#include "components/delta/base.hpp"
#include "components/storage/helper.hpp"

namespace components::delta 
{

template<typename offset_t, typename val_size_t=uint8_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
class Layered : public components::delta::Base<offset_t, val_size_t>
{
public:
	using delta_t = std::unique_ptr<components::delta::Base<offset_t, val_size_t>>;
	Layered(components::storage::Layered<offset_t, true, val_size_t>& storage);
	~Layered() = default;
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

#include "layered.tpp"
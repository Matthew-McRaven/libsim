#include "vector.hpp"

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Layered<offset_t, val_size_t>::Layered(components::storage::Layered<offset_t, true, val_size_t>& storage): 
	components::delta::Base<offset_t, val_size_t>(storage), _deltas()
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Layered<offset_t, val_size_t>::apply_forward()
{
	if(_deltas.empty()) return;
	for(auto& delta : _deltas) delta->apply_forward();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Layered<offset_t, val_size_t>::apply_backward()
{
	if(_deltas.empty()) return;
	for(auto& delta : _deltas) delta->apply_backward();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Layered<offset_t, val_size_t>::clear()
{
	_deltas.clear();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Layered<offset_t, val_size_t>::add_delta(std::unique_ptr<components::delta::Base<offset_t, val_size_t>> delta)
{
	_deltas.emplace_back(std::move(delta));
}
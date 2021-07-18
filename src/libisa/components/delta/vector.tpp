#include "vector.hpp"

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Vector<offset_t, val_size_t>::Vector(components::storage::Base<offset_t, true, val_size_t>& storage): 
	components::delta::Base<offset_t, val_size_t>(storage), _deltas()
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Vector<offset_t, val_size_t>::apply_forward()
{
	if(_deltas.empty()) return;
	// Set implicitly bypasses deltas, so we do not need to worry about applying our delta itself generating deltas.
	// Call .value() to force resolution of possible errors. Perform no error handling, because we can't recover from a bad delta.
	for(const auto &[offset, _, new_value] : _deltas) this->_storage.set(offset, new_value).value();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Vector<offset_t, val_size_t>::apply_backward()
{
	if(_deltas.empty()) return;
	// Set implicitly bypasses deltas, so we do not need to worry about applying our delta itself generating deltas.
	// Call .value() to force resolution of possible errors. Perform no error handling, because we can't recover from a bad delta.
	for(const auto &[offset, old_value, _] : _deltas) this->_storage.set(offset, old_value).value();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Vector<offset_t, val_size_t>::add_delta(offset_t offset, val_size_t old_value, val_size_t new_value)
{
	std::tuple<offset_t, val_size_t, val_size_t> delta = {offset, old_value, new_value};
	_deltas.emplace_back(std::move(delta));
}
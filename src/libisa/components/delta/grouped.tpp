#include "vector.hpp"
// Must include any "friend"-ish storage devices here.
#include "components/storage/layered.hpp"
/***************************
 * Iterator Implementation *
 ***************************/
template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Grouped<offset_t, val_size_t>::impl::impl(const std::vector<delta_t>& deltas, 
	const components::storage::Layered<offset_t, true, val_size_t>* base):
	components::delta::Base<offset_t, val_size_t>::Iterator::AIter(), 
	_rng(), _base(base), _iter()
{
	delta_to_range(deltas);
	for(auto[offset, shared_ptr_storage] : base->contained_storage()) offset_map[shared_ptr_storage.get()] = offset;
	_iter = boost::const_begin(_rng);
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Grouped<offset_t, val_size_t>::impl::impl(const impl& other):
	components::delta::Base<offset_t, val_size_t>::Iterator::AIter(),
	_rng(other._rng), _base(other._base), _iter(other._iter), offset_map(other.offset_map)
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Grouped<offset_t, val_size_t>::impl::next(int n)
{
	_iter = std::next(_iter, n);
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
const typename components::delta::Base<offset_t, val_size_t>::Iterator::value_type
	components::delta::Grouped<offset_t, val_size_t>::impl::deref() const
{
	auto [device, offset, value] = *_iter;
	auto embed_offset = offset_map.at(device);
	return std::make_tuple(_base, offset+embed_offset, value);
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
bool components::delta::Grouped<offset_t, val_size_t>::impl::equal(
	const typename components::delta::Base<offset_t, val_size_t>::Iterator::AIter* rp) const
{
	if(auto as_self = dynamic_cast<const impl*>(rp); as_self) {
		return _rng == as_self->_rng &&
			_iter == as_self->_iter &&
			_base == as_self->_base;
	}
	return false;
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
std::unique_ptr<typename components::delta::Base<offset_t, val_size_t>::Iterator::AIter>
	components::delta::Grouped<offset_t, val_size_t>::impl::clone() const 
{
	return std::make_unique<impl>(*this);
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Grouped<offset_t, val_size_t>::impl::delta_to_range(const std::vector<delta_t>& deltas) 
{
	if(deltas.size() == 0) {
		_rng = any_range();
	} else {
		_rng = any_range(deltas[0]->cbegin(), deltas[0]->cend());
		bool skip_first = true;
		for(auto& delta : deltas) {
			if(skip_first){skip_first=false; continue;} 
			_rng = boost::join(_rng, any_range(delta->cbegin(), delta->cend()));
		}
	}
}

/**************************
 * Storage Implementation *
 **************************/
template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Grouped<offset_t, val_size_t>::Grouped(components::storage::Layered<offset_t, true, val_size_t>* layered): 
	components::delta::Base<offset_t, val_size_t>(), _deltas(), _layered(layered)
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Grouped<offset_t, val_size_t>::apply_forward()
{
	if(_deltas.empty()) return;
	for(auto& delta : _deltas) delta->apply_forward();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Grouped<offset_t, val_size_t>::apply_backward()
{
	if(_deltas.empty()) return;
	for(auto& delta : _deltas) delta->apply_backward();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Grouped<offset_t, val_size_t>::clear()
{
	_deltas.clear();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Grouped<offset_t, val_size_t>::add_delta(std::unique_ptr<components::delta::Base<offset_t, val_size_t>> delta)
{
	_deltas.emplace_back(std::move(delta));
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
typename components::delta::Base<offset_t, val_size_t>::Iterator components::delta::Grouped<offset_t, val_size_t>::cbegin()
{
	auto iter = std::make_unique<impl>(_deltas, _layered);
	return typename components::delta::Base<offset_t, val_size_t>::Iterator(std::move(iter));
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
typename components::delta::Base<offset_t, val_size_t>::Iterator components::delta::Grouped<offset_t, val_size_t>::cend()
{
	auto iter = std::make_unique<impl>(_deltas, _layered);
	return typename components::delta::Base<offset_t, val_size_t>::Iterator(std::move(iter));
}
#include "vector.hpp"
/***************************
 * Iterator Implementation *
 ***************************/
// Template keyword in the middle is necessary for dependantly typed template classes.
// Also note that there are two template<...> lines, one for each level of templates.
// These may not be merged, else weird compile errors (not related to the templates) will follow.
template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
template <class Iter>
components::delta::Vector<offset_t, val_size_t>::template impl<Iter>::impl(Iter iter, const components::storage::Base<offset_t, true, val_size_t>* base):
	components::delta::Base<offset_t, val_size_t>::Iterator::AIter(), _iter(iter), _base(base)
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
template <class Iter>
components::delta::Vector<offset_t, val_size_t>::template impl<Iter>::impl(const impl& other): 
	components::delta::Base<offset_t, val_size_t>::Iterator::AIter(), _iter(other._iter), _base(other._base)
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
template <class Iter>
void components::delta::Vector<offset_t, val_size_t>::template impl<Iter>::next(int n) 
{ 
	_iter = std::next(_iter, n); 
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
template <class Iter>
const typename components::delta::Base<offset_t, val_size_t>::Iterator::value_type
	components::delta::Vector<offset_t, val_size_t>::template impl<Iter>::deref() const
{ 
	auto [offset, _, val] = *_iter;
	auto ret = std::make_tuple(_base, offset, val);
	return ret;
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
template <class Iter>
bool components::delta::Vector<offset_t, val_size_t>::template impl<Iter>::equal(const AIter* rp) const
{ 
	if(auto as_self = dynamic_cast<const impl*>(rp); as_self) {
		return _iter == as_self->_iter && _base == as_self->_base; 
	}
	return false;
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
template <class Iter>
std::unique_ptr<typename components::delta::Base<offset_t, val_size_t>::Iterator::AIter>
	components::delta::Vector<offset_t, val_size_t>::template impl<Iter>::clone() const 
{
	return std::make_unique<impl>(*this);
}

/*************************
 * Vector Implementation *
 *************************/
template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Vector<offset_t, val_size_t>::Vector(components::storage::Base<offset_t, true, val_size_t>& storage): 
	components::delta::Base<offset_t, val_size_t>(), _storage(storage), _deltas()
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
void components::delta::Vector<offset_t, val_size_t>::clear()
{
	_deltas.clear();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
typename components::delta::Base<offset_t, val_size_t>::Iterator components::delta::Vector<offset_t, val_size_t>::cbegin()
{
	auto iter = std::make_unique<impl<decltype(_deltas.begin())>>(_deltas.begin(), &_storage);
	return typename components::delta::Base<offset_t, val_size_t>::Iterator(std::move(iter));
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
typename components::delta::Base<offset_t, val_size_t>::Iterator components::delta::Vector<offset_t, val_size_t>::cend()
{
	auto iter = std::make_unique<impl<decltype(_deltas.end())>>(_deltas.end(), &_storage);
	return typename components::delta::Base<offset_t, val_size_t>::Iterator(std::move(iter));
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Vector<offset_t, val_size_t>::add_delta(offset_t offset, val_size_t old_value, val_size_t new_value)
{
	std::tuple<offset_t, val_size_t, val_size_t> delta = {offset, old_value, new_value};
	_deltas.emplace_back(std::move(delta));
}
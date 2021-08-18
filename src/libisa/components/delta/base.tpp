
template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Base<offset_t, val_size_t>::Iterator::Iterator(std::unique_ptr<AIter> iter): 
	_impl(std::move(iter))
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Base<offset_t, val_size_t>::Iterator::Iterator(const Iterator& r): 
	_impl(r._impl->clone())
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
typename components::delta::Base<offset_t, val_size_t>::Iterator::value_type 
	components::delta::Base<offset_t, val_size_t>::Iterator::operator*() const 
{
	return _impl->deref();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
typename components::delta::Base<offset_t, val_size_t>::Iterator& 
	components::delta::Base<offset_t, val_size_t>::Iterator::operator++() {
	_impl->next(1);
	return *this;
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
typename components::delta::Base<offset_t, val_size_t>::Iterator 
	components::delta::Base<offset_t, val_size_t>::Iterator::operator++(int) 
{
	Iterator tmp = *this;
	++(*this);
	return tmp;
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
bool components::delta::Base<offset_t, val_size_t>::Iterator::operator==(const Iterator& r) const 
{
	return _impl->equal(r._impl.get());
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
bool components::delta::Base<offset_t, val_size_t>::Iterator::operator!=(const Iterator& r) const 
{
	return !(*this == r);
}
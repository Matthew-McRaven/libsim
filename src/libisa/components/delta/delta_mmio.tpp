/*********************************
 * Input Iterator Implementation *
 *********************************/
template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Input<offset_t, val_size_t>::impl::impl(
	std::shared_ptr<typename components::storage::Channel<offset_t, val_size_t>::Endpoint> endpoint, 
	const components::storage::Base<offset_t, true, val_size_t>* base, bool at_end):
	components::delta::Base<offset_t, val_size_t>::Iterator::AIter(), _endpoint(endpoint), _base(base), _at_end(at_end)
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Input<offset_t, val_size_t>::impl::impl(const impl& other):
	components::delta::Base<offset_t, val_size_t>::Iterator::AIter(), _endpoint(other._endpoint->clone()), 
	_base(other._base), _at_end(other._at_end)
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Input<offset_t, val_size_t>::impl::next(int n)
{
	for(int it=0; it<abs(n); it++){
		if(n > 0) {
			if(_endpoint->next_value() == std::nullopt) _at_end = true;
		} else {
			if(_endpoint->unread() == std::nullopt) _at_end = true;
		}
	}
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
const typename components::delta::Base<offset_t, val_size_t>::Iterator::value_type
	components::delta::Input<offset_t, val_size_t>::impl::deref() const
{
	// Will crash if current value is nullopt.
	// This case corresponds to the iterator being at cend(), in which case a dereference should fail anyway.
	return std::make_tuple(_base, 0, *_endpoint->current_value());
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
bool components::delta::Input<offset_t, val_size_t>::impl::equal(const AIter* rp) const
{
	if(auto as_self = dynamic_cast<const impl*>(rp); as_self) {
		return _endpoint->event_id() == as_self->_endpoint->event_id() && 
		_at_end == as_self->_at_end &&
		_base == as_self->_base; 
	}
	return false;
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
std::unique_ptr<typename components::delta::Base<offset_t, val_size_t>::Iterator::AIter>
	components::delta::Input<offset_t, val_size_t>::impl::clone() const 
{
	return std::make_unique<impl>(*this);
}

/******************************
 * Input Delta Implementation *
 ******************************/
template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Input<offset_t, val_size_t>::Input(components::storage::Input<offset_t, true, val_size_t>& storage): 
	components::delta::Base<offset_t, val_size_t>(), 
	_storage(storage), _read_count(0)
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Input<offset_t, val_size_t>::apply_forward()
{
	for(int it = 0; it < _read_count; it++) _storage._endpoint->next_value();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Input<offset_t, val_size_t>::apply_backward()
{
	for(int it = 0; it < _read_count; it++) _storage._endpoint->unread();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Input<offset_t, val_size_t>::clear()
{
	_read_count = 0;
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Input<offset_t, val_size_t>::add_delta()
{
	++_read_count;
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
typename components::delta::Base<offset_t, val_size_t>::Iterator components::delta::Input<offset_t, val_size_t>::cbegin()
{
	auto endpoint = _storage._storage->new_endpoint();
	endpoint->set_to_tail();
	// Must be -1 to account for the fact there is no end sentinel, and the tail value was one of the modified values.
	for(int it=0; it < _read_count-1; it++) endpoint->unread();
	auto iter = std::make_unique<impl>(endpoint, &_storage);
	return typename components::delta::Base<offset_t, val_size_t>::Iterator(std::move(iter));
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
typename components::delta::Base<offset_t, val_size_t>::Iterator components::delta::Input<offset_t, val_size_t>::cend()
{
	auto endpoint = _storage._storage->new_endpoint();
	endpoint->set_to_tail();
	auto iter = std::make_unique<impl>(endpoint, &_storage, true);
	return typename components::delta::Base<offset_t, val_size_t>::Iterator(std::move(iter));
}

/**********************************
 * Output Iterator Implementation *
 **********************************/
template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Output<offset_t, val_size_t>::impl::impl(
	std::shared_ptr<typename components::storage::Channel<offset_t, val_size_t>::Endpoint> endpoint, 
	const components::storage::Base<offset_t, true, val_size_t>* base, bool at_end):
	components::delta::Base<offset_t, val_size_t>::Iterator::AIter(), _endpoint(endpoint), _base(base), _at_end(at_end)
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Output<offset_t, val_size_t>::impl::impl(const impl& other):
	components::delta::Base<offset_t, val_size_t>::Iterator::AIter(), _endpoint(other._endpoint->clone()), 
	_base(other._base), _at_end(other._at_end)
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Output<offset_t, val_size_t>::impl::next(int n)
{
	for(int it=0; it<abs(n); it++){
		if(n > 0) {
			if(_endpoint->next_value() == std::nullopt) _at_end = true;
		} else {
			if(_endpoint->unread() == std::nullopt) _at_end = true;
		}
	}
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
const typename components::delta::Base<offset_t, val_size_t>::Iterator::value_type
	components::delta::Output<offset_t, val_size_t>::impl::deref() const
{
	// Will crash if current value is nullopt.
	// This case corresponds to the iterator being at cend(), in which case a dereference should fail anyway.
	auto value = *(_endpoint->current_value());
	return std::make_tuple(_base, 0, value);
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
bool components::delta::Output<offset_t, val_size_t>::impl::equal(const AIter* rp) const
{
	if(auto as_self = dynamic_cast<const impl*>(rp); as_self) {
		return _endpoint->event_id() == as_self->_endpoint->event_id() && 
			_base == as_self->_base &&
			_at_end == as_self->_at_end; 
	}
	return false;
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
std::unique_ptr<typename components::delta::Base<offset_t, val_size_t>::Iterator::AIter>
	components::delta::Output<offset_t, val_size_t>::impl::clone() const 
{
	return std::make_unique<impl>(*this);
}

/*******************************
 * Output Delta Implementation *
 *******************************/
template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Output<offset_t, val_size_t>::Output(components::storage::Output<offset_t, true, val_size_t>& storage): 
	components::delta::Base<offset_t, val_size_t>(), 
	_storage(storage), _write_count(0)
{

}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Output<offset_t, val_size_t>::apply_forward()
{
	for(int it = 0; it < _write_count; it++) _storage._endpoint->next_value();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Output<offset_t, val_size_t>::apply_backward()
{
	for(int it = 0; it < _write_count; it++) _storage._endpoint->unwrite();
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Output<offset_t, val_size_t>::clear()
{
	_write_count = 0;
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
void components::delta::Output<offset_t, val_size_t>::add_delta()
{
	++_write_count;
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
typename components::delta::Base<offset_t, val_size_t>::Iterator components::delta::Output<offset_t, val_size_t>::cbegin()
{
	auto endpoint = _storage._storage->new_endpoint();
	endpoint->set_to_tail();
	// Must be -1 to account for the fact there is no end sentinel, and the tail value was one of the modified values.
	// Unread rather than unwrite because this particular endpoint ID has never written anything.
	// According to the docs, unread is logically equivalent to previous_event(...).
	for(int it=0; it<_write_count-1; it++) endpoint->unread();
	auto iter = std::make_unique<impl>(endpoint, &_storage);
	return typename components::delta::Base<offset_t, val_size_t>::Iterator(std::move(iter));
}

template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
typename components::delta::Base<offset_t, val_size_t>::Iterator components::delta::Output<offset_t, val_size_t>::cend()
{
	auto endpoint = _storage._storage->new_endpoint();
	endpoint->set_to_tail();
	auto iter = std::make_unique<impl>(endpoint, &_storage, true);
	return typename components::delta::Base<offset_t, val_size_t>::Iterator(std::move(iter));
}
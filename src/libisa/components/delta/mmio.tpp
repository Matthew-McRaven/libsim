// Input
template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Input<offset_t, val_size_t>::Input(components::storage::Input<offset_t, true, val_size_t>& storage): 
	components::delta::Base<offset_t, val_size_t>(storage), 
	_storage(storage)
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

// Output
template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Output<offset_t, val_size_t>::Output(components::storage::Output<offset_t, true, val_size_t>& storage): 
	components::delta::Base<offset_t, val_size_t>(storage), 
	_storage(storage)
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
template <typename offset_t, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
components::delta::Base<offset_t, val_size_t>::Base(components::storage::Base<offset_t, true, val_size_t>& storage): 
	_storage(storage)
{

}

template<typename addr_size_t, typename addr_val_t, typename reg_count_t, typename reg_val_t, typename csr_count_t, 
	typename csr_val_t>
	requires (components::storage::UnsignedIntegral<addr_size_t> && components::storage::Integral<addr_val_t> &&
	components::storage::UnsignedIntegral<reg_count_t> && components::storage::Integral<reg_val_t> &&
	components::storage::UnsignedIntegral<csr_count_t> && components::storage::Integral<csr_val_t>)
components::machine::StepDelta<addr_size_t, addr_val_t, reg_count_t, reg_val_t, csr_count_t, csr_val_t>::StepDelta(
	std::unique_ptr<components::delta::Base<addr_size_t, addr_val_t>> memory, 
	std::unique_ptr<components::delta::Base<reg_count_t, reg_val_t>> registers, 
	std::unique_ptr<components::delta::Base<csr_count_t, csr_val_t>> csrs):
	_delta_memory(std::move(memory)), _delta_reg(std::move(registers)), _delta_csr(std::move(csrs))
{

}

template<typename addr_size_t, typename addr_val_t, typename reg_count_t, typename reg_val_t, typename csr_count_t, 
	typename csr_val_t>
	requires (components::storage::UnsignedIntegral<addr_size_t> && components::storage::Integral<addr_val_t> &&
	components::storage::UnsignedIntegral<reg_count_t> && components::storage::Integral<reg_val_t> &&
	components::storage::UnsignedIntegral<csr_count_t> && components::storage::Integral<csr_val_t>)
void components::machine::StepDelta<addr_size_t, addr_val_t, reg_count_t, reg_val_t, csr_count_t, csr_val_t>::apply_forward()
{
	_delta_memory->apply_forward();
	_delta_reg->apply_forward();
	_delta_csr->apply_forward();
}

template<typename addr_size_t, typename addr_val_t, typename reg_count_t, typename reg_val_t, typename csr_count_t, 
	typename csr_val_t>
	requires (components::storage::UnsignedIntegral<addr_size_t> && components::storage::Integral<addr_val_t> &&
	components::storage::UnsignedIntegral<reg_count_t> && components::storage::Integral<reg_val_t> &&
	components::storage::UnsignedIntegral<csr_count_t> && components::storage::Integral<csr_val_t>)
void components::machine::StepDelta<addr_size_t, addr_val_t, reg_count_t, reg_val_t, csr_count_t, csr_val_t>::apply_backward()
{
	_delta_memory->apply_backward();
	_delta_reg->apply_backward();
	_delta_csr->apply_backward();
}

template<typename addr_size_t, typename addr_val_t, typename reg_count_t, typename reg_val_t, typename csr_count_t, 
	typename csr_val_t>
	requires (components::storage::UnsignedIntegral<addr_size_t> && components::storage::Integral<addr_val_t> &&
	components::storage::UnsignedIntegral<reg_count_t> && components::storage::Integral<reg_val_t> &&
	components::storage::UnsignedIntegral<csr_count_t> && components::storage::Integral<csr_val_t>)
typename components::machine::StepDelta<addr_size_t, addr_val_t, reg_count_t, reg_val_t, csr_count_t, csr_val_t>::addr_delta_range 
components::machine::StepDelta<addr_size_t, addr_val_t, reg_count_t, reg_val_t, csr_count_t, csr_val_t>::memory_delta() const
{
	return addr_delta_range(_delta_memory.cbegin(), _delta_memory.cend());
}

template<typename addr_size_t, typename addr_val_t, typename reg_count_t, typename reg_val_t, typename csr_count_t, 
	typename csr_val_t>
	requires (components::storage::UnsignedIntegral<addr_size_t> && components::storage::Integral<addr_val_t> &&
	components::storage::UnsignedIntegral<reg_count_t> && components::storage::Integral<reg_val_t> &&
	components::storage::UnsignedIntegral<csr_count_t> && components::storage::Integral<csr_val_t>)
typename components::machine::StepDelta<addr_size_t, addr_val_t, reg_count_t, reg_val_t, csr_count_t, csr_val_t>::reg_delta_range 
components::machine::StepDelta<addr_size_t, addr_val_t, reg_count_t, reg_val_t, csr_count_t, csr_val_t>::register_delta() const
{
	return reg_delta_range(_delta_reg.cbegin(), _delta_reg.cend());
}

template<typename addr_size_t, typename addr_val_t, typename reg_count_t, typename reg_val_t, typename csr_count_t, 
	typename csr_val_t>
	requires (components::storage::UnsignedIntegral<addr_size_t> && components::storage::Integral<addr_val_t> &&
	components::storage::UnsignedIntegral<reg_count_t> && components::storage::Integral<reg_val_t> &&
	components::storage::UnsignedIntegral<csr_count_t> && components::storage::Integral<csr_val_t>)
typename components::machine::StepDelta<addr_size_t, addr_val_t, reg_count_t, reg_val_t, csr_count_t, csr_val_t>::csr_delta_range 
components::machine::StepDelta<addr_size_t, addr_val_t, reg_count_t, reg_val_t, csr_count_t, csr_val_t>::csr_delta() const
{
	return csr_delta_range(_delta_csr.cbegin(), _delta_csr.cend());
}
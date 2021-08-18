#pragma once

#include <memory>

#include <outcome.hpp>

#include "components/delta/base.hpp"
#include "components/storage/helper.hpp"
#include "outcome_helper.hpp"

namespace components::machine
{
template<typename addr_size_t, typename addr_val_t, typename reg_count_t, typename reg_val_t, typename csr_count_t, 
	typename csr_val_t>
	requires (components::storage::UnsignedIntegral<addr_size_t> && components::storage::Integral<addr_val_t> &&
	components::storage::UnsignedIntegral<reg_count_t> && components::storage::Integral<reg_val_t> &&
	components::storage::UnsignedIntegral<csr_count_t> && components::storage::Integral<csr_val_t>)
class step_delta
{
public:
	step_delta(std::unique_ptr<components::delta::Base<addr_size_t, addr_val_t>>, 
		std::unique_ptr<components::delta::Base<reg_count_t, reg_val_t>>, 
		std::unique_ptr<components::delta::Base<csr_count_t, csr_val_t>>);
	void apply_forward();
	void apply_backward();
private:
	std::unique_ptr<components::delta::Base<addr_size_t, addr_val_t>> _delta_memory;
	std::unique_ptr<components::delta::Base<reg_count_t, reg_val_t>> _delta_regs;
	std::unique_ptr<components::delta::Base<csr_count_t, csr_val_t>> _delta_csr;

;}
} // End namespace components::machine
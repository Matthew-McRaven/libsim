#pragma once

#include <memory>

#include <boost/range/any_range.hpp>
#include <boost/range/join.hpp>
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
class StepDelta
{
public:
	using addr_delta_range = boost::iterator_range<decltype(((components::delta::Base<addr_size_t, addr_val_t>*)(0))->cbegin())>;
	using reg_delta_range = boost::iterator_range<decltype(((components::delta::Base<addr_size_t, addr_val_t>*)(0))->cbegin())>;
	using csr_delta_range = boost::iterator_range<decltype(((components::delta::Base<addr_size_t, addr_val_t>*)(0))->cbegin())>;

	StepDelta(std::unique_ptr<components::delta::Base<addr_size_t, addr_val_t>>, 
		std::unique_ptr<components::delta::Base<reg_count_t, reg_val_t>>, 
		std::unique_ptr<components::delta::Base<csr_count_t, csr_val_t>>);
	StepDelta(StepDelta&& other) = default;
	void apply_forward();
	void apply_backward();

	addr_delta_range memory_delta() const;
	reg_delta_range register_delta() const;
	csr_delta_range csr_delta() const;
private:
	std::unique_ptr<components::delta::Base<addr_size_t, addr_val_t>> _delta_memory;
	std::unique_ptr<components::delta::Base<reg_count_t, reg_val_t>> _delta_reg;
	std::unique_ptr<components::delta::Base<csr_count_t, csr_val_t>> _delta_csr;

};
} // End namespace components::machine

#include "delta.tpp"
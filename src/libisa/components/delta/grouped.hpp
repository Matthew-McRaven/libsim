#pragma once

#include <boost/range/any_range.hpp>
#include <boost/range/join.hpp>

#include "components/delta/base.hpp"
#include "components/storage/helper.hpp"

namespace components::storage
{
// Forward declare any storage devices that use a grouped delta.
// There must be two way communication between delta and storage device, since delta must remap contained offsets to parent offsets.
// Storage device must talk to delta, since it must be capable of creating new deltas.
template<typename offset_t, bool enable_history, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
class Layered;

} //End namespace components::storage
namespace components::delta 
{


template<typename offset_t, typename val_size_t=uint8_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
class Grouped : public components::delta::Base<offset_t, val_size_t>
{
public:
	using delta_t = std::unique_ptr<components::delta::Base<offset_t, val_size_t>>;
	using value_type = typename components::delta::Base<offset_t, val_size_t>::Iterator::value_type;
	using any_range = boost::any_range<const value_type, boost::forward_traversal_tag, const value_type&, std::ptrdiff_t>;
	struct impl : components::delta::Base<offset_t, val_size_t>::Iterator::AIter
	{
		impl(const std::vector<delta_t>& deltas, 
			const components::storage::Layered<offset_t, true, val_size_t>* base);
		impl(const impl& other);

		void next(int n) override;
		const typename components::delta::Base<offset_t, val_size_t>::Iterator::value_type deref() const override;
		bool equal(const typename components::delta::Base<offset_t, val_size_t>::Iterator::AIter* rp) const override;
		std::unique_ptr<typename components::delta::Base<offset_t, val_size_t>::Iterator::AIter> clone() const override;

		// While bad style, this function helps the constructor initialize _rng. This has been split into another 
		// function, as I anticipate multiple kinds of grouped storage devices that need to be constructed from a 
		// delta vector. This delta->range conversion will be present in any implementation, and should not be repeated.
		void delta_to_range(const std::vector<delta_t>&);
		// Join the vector of deltas into a single iterable, so that our internal iteration API is clean.
		any_range _rng;
		decltype(boost::const_begin(any_range{})) _iter;
		const components::storage::Base<offset_t, true, val_size_t>* _base;
		// Grouped storage devices tell use the mapping between the grouped storage's offsets and the local
		// offsets of each contained storage. However, we lose information about the kind of grouped storage after 
		// the constructor. Therefore, we must cache this information locally.
		std::map<const components::storage::Base<offset_t, true, val_size_t>*, offset_t> offset_map;
	};


	// Does not correspond to a physical storage device, but rather a group of devices. 
	// Therefore, do not take a device as an argument. 
	Grouped(components::storage::Layered<offset_t, true, val_size_t>* layered);
	~Grouped() = default;
	void apply_backward() override;
	void apply_forward() override;
	void clear() override;
	void add_delta(delta_t delta);
	typename components::delta::Base<offset_t, val_size_t>::Iterator cbegin() override;
	typename components::delta::Base<offset_t, val_size_t>::Iterator cend() override;
private:
	// Store all layers' deltas in a vector.
	// Each delta applies to a different storage device.
	std::vector<delta_t> _deltas;
	components::storage::Layered<offset_t, true, val_size_t>* _layered;
};
} // namespace components::delta

#include "grouped.tpp"
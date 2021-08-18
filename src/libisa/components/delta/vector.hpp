#pragma once

#include "components/delta/base.hpp"
#include "components/storage/helper.hpp"

namespace components::delta 
{

template<typename offset_t, typename val_size_t=uint8_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
class Vector : public components::delta::Base<offset_t, val_size_t>
{
public:
	// I don't know what kind of iterator I'm going to be receiving (vector, list, map),
	// all I know is that when I derefence it I get an (offset, old_val, new_val) tuple.
	// So, let's templatize based on the kind of iterator.
	template<class Iter>
	struct impl : components::delta::Base<offset_t, val_size_t>::Iterator::AIter
	{
		using AIter = typename components::delta::Base<offset_t, val_size_t>::Iterator::AIter;
		impl(Iter iter, const components::storage::Base<offset_t, true, val_size_t>* base);
		impl(const impl& other);

		void next(int n) override;
		const typename components::delta::Base<offset_t, val_size_t>::Iterator::value_type deref() const override;
		bool equal(const AIter* rp) const override;
		std::unique_ptr<AIter> clone() const override;

		Iter _iter;
		const components::storage::Base<offset_t, true, val_size_t>* _base;
	};

	Vector(components::storage::Base<offset_t, true, val_size_t>& storage);
	~Vector() = default;
	void apply_backward() override;
	void apply_forward() override;
	void clear() override;
	void add_delta(offset_t offset, val_size_t old_value, val_size_t new_value);
	typename components::delta::Base<offset_t, val_size_t>::Iterator cbegin() override;
	typename components::delta::Base<offset_t, val_size_t>::Iterator cend() override;
private:
	components::storage::Base<offset_t, true, val_size_t>& _storage;
	// Store changes in a vector, sorted from smallest to largest offset.
	// Keep elements with duplicate offsets---skipping them would cause replay to differ from execution.
	std::vector<std::tuple<offset_t, val_size_t /*old*/, val_size_t/*new*/>> _deltas;
	
};
} // namespace components::delta

#include "vector.tpp"
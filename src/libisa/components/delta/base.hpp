#pragma once

#include <memory>

#include <outcome.hpp>

#include "components/storage/helper.hpp"

// Must forward declare for reference to work.
// Must also carry the same requires clauses.

namespace components::storage {
template<typename offset_t, bool enable_history, typename val_size_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
class Base;
}


namespace components::delta 
{



template<typename offset_t, typename val_size_t=uint8_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
class Base
{
public:
	struct Iterator
	{
		// Boilerplate type declarations needed for Iterator to be considered an iterator by C++.
		typedef std::forward_iterator_tag iterator_category;
		typedef std::ptrdiff_t difference_type;
		// Use a const base so we can't mutate Base (easily).
		typedef const std::tuple<const components::storage::Base<offset_t, true, val_size_t>*, offset_t, val_size_t> value_type;
		typedef value_type* pointer;
		typedef value_type& reference;
		// Base class from which our polymorphic iterators will dervive.
		// TODO: Use operators correctly rather than these ad-hoc methods.
		struct AIter
		{
			//ctor / dtor
			virtual ~AIter() = default;
			// Iterator methods.
			virtual void next(int n) = 0;
			virtual const value_type deref() const = 0;
			virtual bool equal(const AIter* other) const = 0;
			virtual std::unique_ptr<AIter> clone() const = 0;
		};


	public:
		//Must be default constructible to appease boost ranges, but creates a meaningless (crashing) iterator.
		Iterator() = default;
		Iterator(std::unique_ptr<AIter> iter);
		Iterator(const Iterator& r);

		// *MUST* return by value rather than by reference. This is because return values are constructed 
		// on-the-fly and not stored anywhere. This would cause dangling references and all sorts of bad problems.
		const value_type operator*() const;
		// Boost ranges need both kinds increment operators.
		Iterator& operator++();
		Iterator operator++(int);

		bool operator==(const Iterator& r) const;
		bool operator != (const Iterator& r) const;

		private:
			std::unique_ptr<AIter> _impl {nullptr};
	};


	virtual ~Base() = default;
	virtual void apply_backward() = 0;
	virtual void apply_forward() = 0;
	virtual void clear() = 0;
	// Provide an interface to iterate over the change set contained within this class.
	Iterator virtual cbegin() = 0;
	Iterator virtual cend() = 0;
	
};
} // namespace components::delta

#include "base.tpp"
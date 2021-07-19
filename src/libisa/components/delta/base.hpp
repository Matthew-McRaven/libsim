#pragma once

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
	Base(components::storage::Base<offset_t, true, val_size_t>& storage);
	virtual ~Base() = default;
	virtual void apply_backward() = 0;
	virtual void apply_forward() = 0;
	virtual void clear() = 0;
protected:
	components::storage::Base<offset_t, true, val_size_t>& _storage;
};
} // namespace components::delta

#include "base.tpp"
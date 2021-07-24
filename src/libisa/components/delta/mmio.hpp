#pragma once

#include "components/delta/base.hpp"
#include "components/storage/helper.hpp"

// Forward declare storage::input, storage::output;
namespace components::storage {
template<typename offset_t, bool enable_history, typename val_size_t>
	requires (UnsignedIntegral<offset_t> && Integral<val_size_t>)
class Input;

template<typename offset_t, bool enable_history, typename val_size_t>
	requires (UnsignedIntegral<offset_t> && Integral<val_size_t>)
class Output;
}

namespace components::delta 
{

template<typename offset_t, typename val_size_t=uint8_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
class Input : public components::delta::Base<offset_t, val_size_t>
{
public:
	Input(components::storage::Input<offset_t, true, val_size_t>& storage);
	~Input() = default;
	void apply_backward() override;
	void apply_forward() override;
	void clear() override;
	void add_delta();
private:
	components::storage::Input<offset_t, true, val_size_t>& _storage;
	val_size_t _read_count;
};

template<typename offset_t, typename val_size_t=uint8_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
class Output : public components::delta::Base<offset_t, val_size_t>
{
public:
	Output(components::storage::Output<offset_t, true, val_size_t>& storage);
	~Output() = default;
	void apply_backward() override;
	void apply_forward() override;
	void clear() override;
	void add_delta();
private:
	components::storage::Output<offset_t, true, val_size_t>& _storage;
	val_size_t _write_count;
};

} // namespace components::delta

#include "mmio.tpp"
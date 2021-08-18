#pragma once

#include "components/delta/base.hpp"
#include "components/storage/helper.hpp"
#include "components/storage/pubsub.hpp"

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
	struct impl : components::delta::Base<offset_t, val_size_t>::Iterator::AIter
	{
		using AIter = typename components::delta::Base<offset_t, val_size_t>::Iterator::AIter;
		impl(std::shared_ptr<typename components::storage::Channel<offset_t, val_size_t>::Endpoint> endpoint, 
			const components::storage::Base<offset_t, true, val_size_t>* base, bool at_end = false);
		impl(const impl& other);

		void next(int n) override;
		const typename components::delta::Base<offset_t, val_size_t>::Iterator::value_type deref() const override;
		bool equal(const AIter* rp) const override;
		std::unique_ptr<AIter> clone() const override;

		std::shared_ptr<typename components::storage::Channel<offset_t, val_size_t>::Endpoint> _endpoint;
		const components::storage::Base<offset_t, true, val_size_t>* _base;
		// There is no sentinel element in the Input storage device. So, we must create a special sentinel flag.
		// It is possible to detecting when stepping forward/backward over the last element, effectively exhausting
		// the sequence. When this occurs, this sentinel flag must be set, or iteration will continue forever.
		bool _at_end;
	};

	Input(components::storage::Input<offset_t, true, val_size_t>& storage);
	~Input() = default;
	void apply_backward() override;
	void apply_forward() override;
	void clear() override;
	void add_delta();
	typename components::delta::Base<offset_t, val_size_t>::Iterator cbegin() override;
	typename components::delta::Base<offset_t, val_size_t>::Iterator cend() override;
private:
	components::storage::Input<offset_t, true, val_size_t>& _storage;
	val_size_t _read_count;
};

template<typename offset_t, typename val_size_t=uint8_t>
	requires (components::storage::UnsignedIntegral<offset_t> && components::storage::Integral<val_size_t>)
class Output : public components::delta::Base<offset_t, val_size_t>
{
public:
	struct impl : components::delta::Base<offset_t, val_size_t>::Iterator::AIter
	{
		using AIter = typename components::delta::Base<offset_t, val_size_t>::Iterator::AIter;
		impl(std::shared_ptr<typename components::storage::Channel<offset_t, val_size_t>::Endpoint> endpoint, 
			const components::storage::Base<offset_t, true, val_size_t>* base, bool at_end = false);
		impl(const impl& other);

		void next(int n) override;
		const typename components::delta::Base<offset_t, val_size_t>::Iterator::value_type deref() const override;
		bool equal(const AIter* rp) const override;
		std::unique_ptr<AIter> clone() const override;

		// This endpoint has a unique ID not shared with any endpoints that produced data.
		// Therefore, it would be dangerous to use the "unwrite" method, even though Output deals in writes.
		std::shared_ptr<typename components::storage::Channel<offset_t, val_size_t>::Endpoint> _endpoint;
		const components::storage::Base<offset_t, true, val_size_t>* _base;
		// There is no sentinel element in the Output storage device. So, we must create a special sentinel flag.
		// It is possible to detecting when stepping forward/backward over the last element, effectively exhausting
		// the sequence. When this occurs, this sentinel flag must be set, or iteration will continue forever.
		bool _at_end;
	};

	Output(components::storage::Output<offset_t, true, val_size_t>& storage);
	~Output() = default;
	void apply_backward() override;
	void apply_forward() override;
	void clear() override;
	void add_delta();
	typename components::delta::Base<offset_t, val_size_t>::Iterator cbegin() override;
	typename components::delta::Base<offset_t, val_size_t>::Iterator cend() override;
private:
	components::storage::Output<offset_t, true, val_size_t>& _storage;
	val_size_t _write_count;
};

} // namespace components::delta

#include "delta_mmio.tpp"
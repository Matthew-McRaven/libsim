#pragma once

#include <outcome.hpp>

#include "components/storage/mmio.hpp"
#include "outcome_helper.hpp"

namespace components::machine
{
// Forward declare interface so that locker will work correctly.
template <typename address_size_t, bool enable_history, typename val_size_t, typename memory_vector_t>
class MachineProcessorInterface;

// Use RAII to begin/end a transaction.
// This is super helpful since a there's too many paths through the processor caused by potential disappointment.
template <typename address_size_t, bool enable_history, typename val_size_t, typename memory_vector_t>
struct TransactionLocker
{
	TransactionLocker(MachineProcessorInterface<address_size_t, enable_history, val_size_t, memory_vector_t>& processor): _processor(processor)
	{
		_processor.begin_transaction();
	}
	// Can't copy this object!
	TransactionLocker(const TransactionLocker&) = delete;
	TransactionLocker(TransactionLocker&&) = default;
	TransactionLocker& operator=(const TransactionLocker&) = delete;
	TransactionLocker& operator=(TransactionLocker&&) = default;

	~TransactionLocker()
	{
		_processor.end_transaction();
	}
private:
	MachineProcessorInterface<address_size_t, enable_history, val_size_t, memory_vector_t>& _processor;
};

// Use RAII to begin/end a transaction.
// This is super helpful since a there's too many paths through the processor caused by potential disappointment.
template <typename address_size_t, bool enable_history, typename val_size_t, typename memory_vector_t>
struct InstructionLocker
{
	InstructionLocker(MachineProcessorInterface<address_size_t, enable_history, val_size_t, memory_vector_t>& processor): _processor(processor)
	{
		_processor.begin_instruction();
	}
	// Can't copy this object!
	InstructionLocker(const InstructionLocker&) = delete;
	InstructionLocker(InstructionLocker&&) = default;
	InstructionLocker& operator=(const InstructionLocker&) = delete;
	InstructionLocker& operator=(InstructionLocker&&) = default;

	~InstructionLocker()
	{
		_processor.end_instruction();
	}
private:
	MachineProcessorInterface<address_size_t, enable_history, val_size_t, memory_vector_t>& _processor;
};

template <typename address_size_t, bool enable_history, typename memory_val_size_t,typename memory_vector_t>
class MachineProcessorInterface
{
public:
	virtual ~MachineProcessorInterface() = default;
	virtual bool halted() const = 0;
	virtual result<memory_val_size_t> get_memory(address_size_t address) const = 0;
	virtual result<void> set_memory(address_size_t address, memory_val_size_t value) = 0;
	virtual result<memory_val_size_t> read_memory(address_size_t address) const = 0;
	virtual result<void> write_memory(address_size_t address, memory_val_size_t value) = 0;
	virtual address_size_t max_offset() const = 0;

	// Don't allow explicit access to the transaction begin/end, as it's very easy to forget to release it.
	TransactionLocker<address_size_t, enable_history, memory_val_size_t, memory_vector_t> acquire_transaction_lock();

	// Don't allow explicit access to the instruction begin/end, as it's very easy to forget to release it.
	InstructionLocker<address_size_t, enable_history, memory_val_size_t, memory_vector_t> acquire_instruction_lock();

	// Needed to undo an in-progress instruction.
	virtual result<void> unwind_active_instruction() = 0;

	// Needed to implement system calls.
	virtual address_size_t address_from_vector(memory_vector_t vector) const = 0;

	// Step back serialization / update querries.
	virtual uint64_t current_time() const = 0;
	virtual result<void> save_deltas() = 0;
	virtual result<void> clear_deltas() = 0;
	// TODO: Determine how to flatten multiple delta iterators in to a single cohesive one.
	virtual void* deltas_between(uint64_t start, uint64_t end) const = 0;

private:
	// Needed for cache model to work.
	virtual void begin_transaction() = 0;
	virtual void end_transaction() = 0;
	friend class TransactionLocker<address_size_t, enable_history, memory_val_size_t, memory_vector_t>;

	// Needed for instruction statistics to work.
	virtual void begin_instruction() = 0;
	virtual void end_instruction() = 0;
	friend class InstructionLocker<address_size_t, enable_history, memory_val_size_t, memory_vector_t>;


};
} // namespace components::machine

#include "interface.tpp"
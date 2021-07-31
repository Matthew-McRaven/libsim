template <typename address_size_t, typename memory_val_size_t, typename memory_vector_t>
components::machine::TransactionLocker<address_size_t, memory_val_size_t, memory_vector_t>
components::machine::MachineProcessorInterface<address_size_t, memory_val_size_t, memory_vector_t>::acquire_transaction_lock()
{
	return components::machine::TransactionLocker<address_size_t, memory_val_size_t, memory_vector_t>(*this);
}

template <typename address_size_t, typename memory_val_size_t, typename memory_vector_t>
components::machine::InstructionLocker<address_size_t, memory_val_size_t, memory_vector_t>
components::machine::MachineProcessorInterface<address_size_t, memory_val_size_t, memory_vector_t>::acquire_instruction_lock()
{
	return components::machine::InstructionLocker<address_size_t, memory_val_size_t, memory_vector_t>(*this);
}
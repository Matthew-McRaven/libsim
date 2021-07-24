#pragma once

#include <outcome.hpp>

#include "components/machine/interace.hpp"
#include "outcome_helper.hpp"

namespace step
{
enum class type
{
	kSingle,
	kStepInto,
	kStepOut,
	kStepOver,
	kStepUntilDone,
};

enum class direction
{
	kForward,
	kBackward,
};
}

namespace components::machine
{

template <typename CPU_t, typename MainMemory_t, typename memory_vector_t>
class VirtualMachine : 
	public components::machine::MachineProcessorInterface<MainMemory_t::address_size_t, MainMemory_t::val_size_t, 
	memory_vector_t>
{
public:
	VirtualMachine(std::shared_ptr<CPU_t> cpu, std::shared_ptr<MainMemory_t> memory);
	
	void step(step_type step, step_direction direction);
	
	MainMemory_t::offset_size_t address_from_vector(memory_vector_t vector);

	// Access memory
	void clear_memory()
	result<MainMemory_t::val_size_t> get_memory(MainMemory_t::offset_size_t address);
	result<void> set_memory(MainMemory_t::offset_size_t address, MainMemory_t::val_size_t value);
	result<MainMemory_t::val_size_t> read_memory(MainMemory_t::offset_size_t address);
	result<void> write_memory(MainMemory_t::offset_size_t address, MainMemory_t::val_size_t value);
protected:
	// Used to implement step-over / step-into / step-out-of.
	virtual result<void> step_while(std::function<bool(void)> condition) = 0;
private:
	void* _delta_holder;
	std::shared_ptr<CPU_t> _cpu;
	std::shared_ptr<MainMemory_t> _memory;
};

} // namespace components::machine
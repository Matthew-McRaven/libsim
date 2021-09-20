#pragma once

#include <functional>
#include <memory>
#include <stdint.h>

#include <outcome.hpp>

#include "outcome_helper.hpp"

namespace step
{
enum class Type
{
	kSingle,
	kStepInto,
	kStepOut,
	kStepOver,
	kStepUntilDone,
};

enum class Direction
{
	kForward,
	kBackward,
};
enum class Result
{
	kHalted,     // The machine stopped execution by writing to the pwrOff port, and may not be resumed.
	kBreakpoint, // The machine stopped execution because of a breakpoint, and may be resumed.
	kNeedsMMI,  // The machine stopped execution due to lack of MMI, and it may be resumed upon enqueueing further data.
	kNominal, // The machine paused because a single step was finished. It may be immediately resumed by calling step().
};
}

namespace components::machine {

template <typename address_size_t, 
	typename register_number_t, typename register_size_t, 
	typename csr_number_t, typename csr_size_t,
	bool enable_history=true>
class ProcessorModel {
public:
	// C.67 Supprress copy/move in polymorphic base class.
	virtual ~ProcessorModel() = default;


	// Interface that must be implemented by deriving processor models
	virtual result<step::Result> step() = 0;
	virtual bool can_step_into() const = 0;
	virtual uint16_t call_depth() const = 0;

	// Set up / tear down
	virtual void init() = 0;
	virtual void debug(bool) = 0;
	virtual void clear(register_size_t reg_fill, csr_size_t csr_fill) = 0;

	// Read / write registers
	virtual register_size_t read_register(register_number_t reg_number) const = 0;
	virtual void write_register(register_number_t reg_number, register_size_t value) = 0;
	virtual register_number_t register_count() const = 0;

	virtual csr_size_t read_csr(csr_number_t csr_number) const = 0;
	virtual void write_csr(csr_number_t csr_number, csr_size_t value) = 0;
	virtual csr_number_t csr_count() const = 0;

	// Statistics
	virtual uint64_t cycle_count() const = 0;
	virtual uint64_t instruction_count() const = 0;

	// Breakpoints
	virtual void add_breakpoint(address_size_t address) = 0;
	// Returns true if address had a breakpoint, false otherwise. Either way, no breakpoint shall exist at address.
	virtual bool remove_breakpoint(address_size_t address) = 0; 
	virtual void remove_all_breakpoints() = 0;
	 
	virtual  result<std::unique_ptr<components::delta::Base<register_number_t, register_size_t>>> take_register_delta() = 0;
	virtual  result<std::unique_ptr<components::delta::Base<csr_number_t, csr_size_t>>> take_csr_delta() = 0;
	// Needed to gather all deltas since last step() call.
	virtual uint64_t last_step_time() const = 0;
};

template <typename processor_t>
result<step::Result> step(std::shared_ptr<processor_t> cpu, step::Type step, step::Direction direction);
result<step::Result> step_while(std::function<bool(void)> condition);
}

#include "processor_model.tpp"
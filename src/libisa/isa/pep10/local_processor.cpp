#include "isa/pep10/local_processor.hpp"

#include <iostream>

#include <magic_enum.hpp>

isa::pep10::LocalProcessor::LocalProcessor(
	components::machine::MachineProcessorInterface<uint16_t, uint8_t, isa::pep10::memory_vectors>& owner):
	_owner(owner),
	_registers(std::make_shared<components::storage::Block<uint8_t, true, uint16_t>>(
		magic_enum::enum_count<isa::pep10::Register>()
	)),
	_csrs(std::make_shared<components::storage::Block<uint8_t, true, bool>>(
		magic_enum::enum_count<isa::pep10::CSR>()
	))
{

}

result<bool> isa::pep10::LocalProcessor::step()
{
	using ::isa::pep10::read_register;
	using ::isa::pep10::write_register;
	auto i_locker = _owner.acquire_instruction_lock();

	// Load PC.
	uint16_t pc = read_register(*this, Register::PC); 

	// Load instruction spec from memory.
	auto is = read_byte(pc);
	if(is.has_failure()) {
		_owner.unwind_active_instruction();
		// Must clone to convert error from reference to value.
		return is.error().clone();
	}
	write_register(*this, Register::IS, is.value());
	
	// Increment PC
	write_register(*this, Register::PC, ++pc);

	if(isa::pep10::is_opcode_unary(is.value())) {
		auto success = unary_dispatch(is.value());
		if(success.has_failure()) {
			_owner.unwind_active_instruction();
			return success.error().clone();
		}
	} else {
		// Load operand specifier from memory.
		auto os = read_word(pc);
		if(os.has_failure()) {
			_owner.unwind_active_instruction();
			// Must clone to convert error from reference to value.
			return os.error().clone();
		}
		write_register(*this, Register::OS, os.value());

		// Increment PC
		write_register(*this, Register::PC, pc+2);

		auto success = nonunary_dispatch(is.value(), os.value());
		if(success.has_failure()) {
			_owner.unwind_active_instruction();
			return success.error().clone();
		}
	}
	++_cycle_count;
	// TODO: Return if the machine is still running.
	return true;
	//return halted();
}

bool isa::pep10::LocalProcessor::can_step_into() const
{
	throw std::logic_error("Not implemented.");
}

uint16_t isa::pep10::LocalProcessor::call_depth() const
{
	throw std::logic_error("Not implemented.");
}


void isa::pep10::LocalProcessor::init()
{
	_cycle_count = 0;
	_registers->clear(0);
	_csrs->clear(false);
}

void isa::pep10::LocalProcessor::debug(bool)
{
	throw std::invalid_argument("Pep/10 ISA model is not yet implemented.");
}

void isa::pep10::LocalProcessor::clear()
{
	throw std::invalid_argument("Pep/10 ISA model is not yet implemented.");
}

// Read / write registers
uint16_t isa::pep10::LocalProcessor::read_register(uint8_t reg_number) const
{
	return _registers->read(reg_number).value();
}

void isa::pep10::LocalProcessor::write_register(uint8_t reg_number, uint16_t value)
{
	_registers->write(reg_number, value).value();
}

uint8_t isa::pep10::LocalProcessor::register_count() const
{
	// Max offset is 0 indexed, whereas count is 1 indexed.
	return _registers->max_offset()+1;
}

bool isa::pep10::LocalProcessor::read_csr(uint8_t csr_number) const
{
	return _csrs->read(csr_number).value();
}

void isa::pep10::LocalProcessor::write_csr(uint8_t csr_number, bool value)
{
	_csrs->write(csr_number, value).value();
}

uint8_t isa::pep10::LocalProcessor::csr_count() const
{
	// Max offset is 0 indexed, whereas count is 1 indexed.
	return _csrs->max_offset()+1;
}


// Statistics
uint64_t isa::pep10::LocalProcessor::cycle_count() const
{
	return _cycle_count;
}

uint64_t isa::pep10::LocalProcessor::instruction_count() const
{
	return _cycle_count;
}

// Todo: How do I coallate deltas for the CPU register bank
void* isa::pep10::LocalProcessor::take_delta()
{
	throw std::logic_error("Not implemented.");
}

result<void> isa::pep10::LocalProcessor::unary_dispatch(uint8_t is)
{
	using ::isa::pep10::read_register;
	using ::isa::pep10::write_register;
	using ::isa::pep10::read_NZVC;

	auto [instr, addr] = isa::pep10::isa_definition::get_definition().riproll[is];
	uint16_t temp_word, sp, acc, idx, vector_value;
	uint8_t temp_byte;
	result<uint16_t> outcome_word = result<uint16_t>(OUTCOME_V2_NAMESPACE::in_place_type<uint16_t>);
	result<uint8_t> outcome_byte = result<uint8_t>(OUTCOME_V2_NAMESPACE::in_place_type<uint8_t>);
	result<void> outcome_void = result<void>(OUTCOME_V2_NAMESPACE::in_place_type<void>);

	sp = read_register(*this, Register::SP);
	acc = read_register(*this, Register::A);
	idx = read_register(*this, Register::X);
    switch(instr->mnemonic) {
	case instruction_mnemonic::RET:
		outcome_word = std::move(read_word(sp));
		if(outcome_word.has_failure()) return outcome_word.error().clone();
		write_register(*this, Register::PC, outcome_word.value());
		write_register(*this, Register::SP, sp+2);
		break;

	case instruction_mnemonic::SRET:
		// Only perform single byte read as status bits do not span full word.
		outcome_byte = std::move(read_byte(sp));
		if(outcome_byte.has_failure()) return outcome_byte.error().clone();
		// Function will automatically mask out bits that don't matter
		write_packed_NZVC(*this, outcome_byte.value());
		
        outcome_word = std::move(read_word(sp + 1));
		if(outcome_word.has_failure()) return outcome_word.error().clone();
		write_register(*this, Register::A, outcome_word.value());

        outcome_word = std::move(read_word(sp + 3));
		if(outcome_word.has_failure()) return outcome_word.error().clone();
		write_register(*this, Register::X, outcome_word.value());

        outcome_word = std::move(read_word(sp + 5));
		if(outcome_word.has_failure()) return outcome_word.error().clone();
		write_register(*this, Register::PC, outcome_word.value());

        outcome_word = std::move(read_word(sp + 7));
		if(outcome_word.has_failure()) return outcome_word.error().clone();
		write_register(*this, Register::SP, outcome_word.value());
        break;

	case instruction_mnemonic::MOVSPA:
		write_register(*this, Register::A, sp);
		break;
	case instruction_mnemonic::MOVASP:
		write_register(*this, Register::SP, acc);
		break;

	case instruction_mnemonic::MOVFLGA:
		write_register(*this, Register::A, ::isa::pep10::read_packed_NZVC(*this));
		break;
	case instruction_mnemonic::MOVAFLG:
		write_packed_NZVC(*this, acc);
		break;

	case instruction_mnemonic::MOVTA:
		temp_word = read_register(*this, Register::TR);
		write_register(*this, Register::A, temp_word);
		break;

	case instruction_mnemonic::NOP:
		break;

	case instruction_mnemonic::NOTA:
		acc = ~acc;
		write_register(*this, Register::A, acc);
		write_NZVC(*this, CSR::N, acc & 0x8000);
		write_NZVC(*this, CSR::Z, acc == 0x0);
		break;
	case instruction_mnemonic::NOTX:break;
		idx = ~idx;
		write_register(*this, Register::X, idx);
		write_NZVC(*this, CSR::N, idx & 0x8000);
		write_NZVC(*this, CSR::Z, idx == 0x0);
		break;

	case instruction_mnemonic::NEGA:
		acc = ~acc + 1;
		write_register(*this, Register::A, acc);
		write_NZVC(*this, CSR::N, acc & 0x8000);
		write_NZVC(*this, CSR::Z, acc == 0x0);
		write_NZVC(*this, CSR::V, acc == 0x8000);
		break;
	case instruction_mnemonic::NEGX:
		idx = ~idx + 1;
		write_register(*this, Register::X, idx);
		write_NZVC(*this, CSR::N, idx & 0x8000);
		write_NZVC(*this, CSR::Z, idx == 0x0);
		write_NZVC(*this, CSR::V, idx == 0x8000);
		break;

	case instruction_mnemonic::ASLA:
		// Store in temp, because we need acc for status bit computation.
		temp_word = static_cast<uint16_t>(acc<<1);
		write_register(*this, Register::A, temp_word);
		// Is negative if high order bit is 1.
        write_NZVC(*this, CSR::N, temp_word & 0x8000);
         // Is zero if all bits are 0's.
        write_NZVC(*this, CSR::Z, temp_word == 0);
        // Signed overflow occurs when the starting & ending values of the high order bit differ (a xor temp == 1).
        // Then shift the result over by 15 places to only keep high order bit (which is the sign).
        write_NZVC(*this, CSR::V, (acc ^ temp_word) >> 15);
        // Carry out if register starts with high order 1.
        write_NZVC(*this, CSR::C, acc & 0x8000);
		break;
	case instruction_mnemonic::ASLX:
		// Store in temp, because we need acc for status bit computation.
		temp_word = static_cast<uint16_t>(idx<<1);
		write_register(*this, Register::X, temp_word);
		// Is negative if high order bit is 1.
        write_NZVC(*this, CSR::N, temp_word & 0x8000);
         // Is zero if all bits are 0's.
        write_NZVC(*this, CSR::Z, temp_word == 0);
        // Signed overflow occurs when the starting & ending values of the high order bit differ (a xor temp == 1).
        // Then shift the result over by 15 places to only keep high order bit (which is the sign).
        write_NZVC(*this, CSR::V, (idx ^ temp_word) >> 15);
        // Carry out if register starts with high order 1.
        write_NZVC(*this, CSR::C, idx & 0x8000);
		break;

	case instruction_mnemonic::ASRA:
		// Shift all bits to the right by 1 position. Since using unsigned shift, must explicitly
        // perform sign extension by hand.
        temp_word = static_cast<uint16_t>(acc >> 1 | ((acc & 0x8000) ? 1<<15 : 0));
		write_register(*this, Register::A, temp_word);
		// Is negative if high order bit is 1.
        write_NZVC(*this, CSR::N, temp_word & 0x8000);
         // Is zero if all bits are 0's.
        write_NZVC(*this, CSR::Z, temp_word == 0);
        // Carry out if register starts with low order 1.
        write_NZVC(*this, CSR::C, acc & 0x1);
        break;
	case instruction_mnemonic::ASRX:
		// Shift all bits to the right by 1 position. Since using unsigned shift, must explicitly
        // perform sign extension by hand.
        temp_word = static_cast<uint16_t>(idx >> 1 | ((idx & 0x8000) ? 1<<15 : 0));
        write_register(*this, Register::X, temp_word);                                                       
		// Is negative if high order bit is 1.
        write_NZVC(*this, CSR::N, temp_word & 0x8000);
         // Is zero if all bits are 0's.
        write_NZVC(*this, CSR::Z, temp_word == 0);
        // Carry out if register starts with low order 1.
        write_NZVC(*this, CSR::C, idx & 0x1);
        break;

	case instruction_mnemonic::ROLA:
		// Shift the carry in to low order bit.
		temp_word = static_cast<uint16_t> (acc << 1 | (read_NZVC(*this, CSR::C) ? 1 : 0));
        write_register(*this, Register::A, temp_word);
		// Carry out if register starts with high order 1.                       
		write_NZVC(*this, CSR::C, acc & 0x8000);
        break;
	case instruction_mnemonic::ROLX:
		// Shift the carry in to low order bit.
		temp_word = static_cast<uint16_t> (idx << 1 | (read_NZVC(*this, CSR::C) ? 1 : 0));
        write_register(*this, Register::X, temp_word);      
		// Carry out if register starts with high order 1.                       
		write_NZVC(*this, CSR::C, idx & 0x8000);
        break;

	case instruction_mnemonic::RORA:
		// Shift the carry in to high order bit.
		temp_word = static_cast<uint16_t> (acc >> 1 | (read_NZVC(*this, CSR::C) ? 1<<15 : 0));
        write_register(*this, Register::A, temp_word);      
		// Carry out if register starts with low order 1.                       
		write_NZVC(*this, CSR::C, acc & 0x1);
        break;
	case instruction_mnemonic::RORX:
		// Shift the carry in to high order bit.
		temp_word = static_cast<uint16_t> (idx >> 1 | (read_NZVC(*this, CSR::C) ? 1<<15 : 0));
        write_register(*this, Register::X, temp_word);      
		// Carry out if register starts with low order 1.                       
		write_NZVC(*this, CSR::C, idx & 0x1);
        break;

	case instruction_mnemonic::SCALL:
	// TODO: Intentional fallthrough annotation.
	case instruction_mnemonic::USCALL:
		vector_value = _owner.address_from_vector(memory_vectors::SYSTEM_STACK);
		outcome_word = std::move(read_word(vector_value));
		if(outcome_word.has_failure()) return outcome_word.error().clone();

		temp_word = outcome_word.value();

		// Writes IS to mem[T-1].
		outcome_void = std::move(write_byte(temp_word - 1, read_register(*this, Register::IS)));
		if(outcome_void.has_failure()) return outcome_void.error().clone();

        // Writes SP to mem[T-2], mem[T-3].
		outcome_void = std::move(write_word(temp_word - 3, read_register(*this, Register::SP)));
		if(outcome_void.has_failure()) return outcome_void.error().clone();

        // Writes to mem[T-4], mem[T-5].
		outcome_void = std::move(write_word(temp_word - 5, read_register(*this, Register::PC)));
		if(outcome_void.has_failure()) return outcome_void.error().clone();

        // Writes to mem[T-6], mem[T-7].
		outcome_void = std::move(write_word(temp_word - 7, read_register(*this, Register::X)));
		if(outcome_void.has_failure()) return outcome_void.error().clone();

        // Writes to mem[T-8], mem[T-9].
		outcome_void = std::move(write_word(temp_word - 9, read_register(*this, Register::A)));
		if(outcome_void.has_failure()) return outcome_void.error().clone();

        // Writes NZVC to mem[T-10].
		outcome_void = std::move(write_byte(temp_word - 10, ::isa::pep10::read_packed_NZVC(*this)));
		if(outcome_void.has_failure()) return outcome_void.error().clone();
		write_register(*this, Register::SP, temp_word - 10);

		vector_value = _owner.address_from_vector(memory_vectors::TRAP);
		outcome_word = std::move(read_word(vector_value));
		if(outcome_word.has_failure()) return outcome_word.error().clone();
		write_register(*this, Register::PC, outcome_word.value());    
		break;
	default:
		throw std::invalid_argument("Illegal instruction in unary decoder.");
	}
	return result<void>(OUTCOME_V2_NAMESPACE::in_place_type<void>);
}

result<void> isa::pep10::LocalProcessor::nonunary_dispatch(uint8_t is, uint16_t os)
{
	using ::isa::pep10::read_register;
	using ::isa::pep10::write_register;
	using ::isa::pep10::read_NZVC;
	using ::isa::pep10::write_NZVC;
	using ::isa::pep10::read_packed_NZVC;
	using ::isa::pep10::write_packed_NZVC;

	auto [instr, addr] = isa::pep10::isa_definition::get_definition().riproll[is];
	uint16_t temp_word, sp, acc, idx, vector_value;
	uint8_t temp_byte;
	result<uint16_t> outcome_word = result<uint16_t>(OUTCOME_V2_NAMESPACE::in_place_type<uint16_t>);
	result<uint8_t> outcome_byte = result<uint8_t>(OUTCOME_V2_NAMESPACE::in_place_type<uint8_t>);
	result<void> outcome_void = result<void>(OUTCOME_V2_NAMESPACE::in_place_type<void>);

	sp = read_register(*this, Register::SP);
	acc = read_register(*this, Register::A);
	idx = read_register(*this, Register::X);

	result<uint16_t> result_decoded_operand = result<uint16_t>(OUTCOME_V2_NAMESPACE::in_place_type<uint16_t>);
	if(is_store(is)) result_decoded_operand = decode_store_operand(*instr, addr, os);
	else result_decoded_operand = decode_load_operand(*instr, addr, os);
	if(result_decoded_operand.has_error()) return result_decoded_operand.error().clone();
	uint16_t decoded_operand = result_decoded_operand.value();

	switch (instr->mnemonic) {
	case instruction_mnemonic::BR:
		write_register(*this, Register::PC, decoded_operand);
		break;
	case instruction_mnemonic::BRLE:
		if(read_NZVC(*this, CSR::N) || read_NZVC(*this, CSR::Z)) {
			write_register(*this, Register::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRLT:
		if(read_NZVC(*this, CSR::N)) {
			write_register(*this, Register::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BREQ:
		if(read_NZVC(*this, CSR::Z)) {
			write_register(*this, Register::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRNE:
		if(!read_NZVC(*this, CSR::Z)) {
			write_register(*this, Register::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRGE:
		if(!read_NZVC(*this, CSR::N)) {
			write_register(*this, Register::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRGT:
		if(!read_NZVC(*this, CSR::N) && !read_NZVC(*this, CSR::Z) ) {
			write_register(*this, Register::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRV:
		if(read_NZVC(*this, CSR::V)) {
			write_register(*this, Register::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRC:
		if(read_NZVC(*this, CSR::C)) {
			write_register(*this, Register::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::CALL:
        sp -= 2;
		outcome_void = std::move(write_word(sp, read_register(*this, Register::PC)));
		if(outcome_void.has_error()) return outcome_void.error().clone();

		// Load decoded operand as new PC.
		write_register(*this, Register::PC, decoded_operand);
		// Update SP with new value
		write_register(*this, Register::SP, sp);
        break;
	case instruction_mnemonic::ADDSP:
		write_register(*this, Register::SP, sp+decoded_operand);
		break;
	case instruction_mnemonic::SUBSP:
		write_register(*this, Register::SP, sp-decoded_operand);
		break;

	case instruction_mnemonic::ADDA:
		// The result is the decoded operand specifier plus the accumulator
        temp_word = acc + decoded_operand;
		write_register(*this, Register::A, temp_word);
        // Is negative if high order bit is 1.
		write_NZVC(*this, CSR::N, temp_word & 0x8000);
        // Is zero if all bits are 0's.
		write_NZVC(*this, CSR::Z, temp_word == 0);
        // There is a signed overflow iff the high order bits of the register and operand
        //are the same, and one input & the output differ in sign.
        // >> Shifts in 0's (unsigned shorts), so after shift, only high order bit remain.
		write_NZVC(*this, CSR::V, (~(acc ^ decoded_operand) & (acc ^ temp_word)) >> 15);
        // Carry out iff result is unsigned less than register or operand.
		write_NZVC(*this, CSR::C, temp_word < acc  || temp_word < decoded_operand);
        break;
	case instruction_mnemonic::ADDX:
		// The result is the decoded operand specifier plus the accumulator
        temp_word = idx + decoded_operand;
		write_register(*this, Register::X, temp_word);
        // Is negative if high order bit is 1.
		write_NZVC(*this, CSR::N, temp_word & 0x8000);
        // Is zero if all bits are 0's.
		write_NZVC(*this, CSR::Z, temp_word == 0);
        // There is a signed overflow iff the high order bits of the register and operand
        //are the same, and one input & the output differ in sign.
        // >> Shifts in 0's (unsigned shorts), so after shift, only high order bit remain.
		write_NZVC(*this, CSR::V, (~(idx ^ decoded_operand) & (idx ^ temp_word)) >> 15);
        // Carry out iff result is unsigned less than register or operand.
		write_NZVC(*this, CSR::C, temp_word < idx  || temp_word < decoded_operand);
        break;
	case instruction_mnemonic::SUBA:
		// The result is the decoded operand specifier plus the accumulator
        temp_word = acc + ~decoded_operand + 1;
		write_register(*this, Register::A, temp_word);
        // Is negative if high order bit is 1.
		write_NZVC(*this, CSR::N, temp_word & 0x8000);
        // Is zero if all bits are 0's.
		write_NZVC(*this, CSR::Z, temp_word == 0);
        // There is a signed overflow iff the high order bits of the register and operand
        //are the same, and one input & the output differ in sign.
        // >> Shifts in 0's (unsigned shorts), so after shift, only high order bit remain.
		write_NZVC(*this, CSR::V, (~(acc ^ decoded_operand) & (acc ^ temp_word)) >> 15);
        // Carry out iff result is unsigned less than register or operand.
		write_NZVC(*this, CSR::C, temp_word < acc  || temp_word < decoded_operand);
        break;
	case instruction_mnemonic::SUBX:
		// The result is the decoded operand specifier plus the accumulator
        temp_word = idx + ~decoded_operand + 1;
		write_register(*this, Register::X, temp_word);
        // Is negative if high order bit is 1.
		write_NZVC(*this, CSR::N, temp_word & 0x8000);
        // Is zero if all bits are 0's.
		write_NZVC(*this, CSR::Z, temp_word == 0);
        // There is a signed overflow iff the high order bits of the register and operand
        //are the same, and one input & the output differ in sign.
        // >> Shifts in 0's (unsigned shorts), so after shift, only high order bit remain.
		write_NZVC(*this, CSR::V, (~(idx ^ decoded_operand) & (idx ^ temp_word)) >> 15);
        // Carry out iff result is unsigned less than register or operand.
		write_NZVC(*this, CSR::C, temp_word < idx  || temp_word < decoded_operand);
        break;

	case instruction_mnemonic::ANDA:
		temp_word = acc & decoded_operand;
		write_register(*this, Register::A, temp_word);
		// Is negative if high order bit is 1.
		write_NZVC(*this, CSR::N, temp_word & 0x8000);
        // Is zero if all bits are 0's.
		write_NZVC(*this, CSR::Z, temp_word == 0);
		break;
	case instruction_mnemonic::ANDX:
		temp_word = idx & decoded_operand;
		write_register(*this, Register::X, temp_word);
		// Is negative if high order bit is 1.
		write_NZVC(*this, CSR::N, temp_word & 0x8000);
        // Is zero if all bits are 0's.
		write_NZVC(*this, CSR::Z, temp_word == 0);
		break;
	case instruction_mnemonic::ORA:
		temp_word = acc | decoded_operand;
		write_register(*this, Register::A, temp_word);
		// Is negative if high order bit is 1.
		write_NZVC(*this, CSR::N, temp_word & 0x8000);
        // Is zero if all bits are 0's.
		write_NZVC(*this, CSR::Z, temp_word == 0);
		break;
	case instruction_mnemonic::ORX:
		temp_word = idx | decoded_operand;
		write_register(*this, Register::X, temp_word);
		// Is negative if high order bit is 1.
		write_NZVC(*this, CSR::N, temp_word & 0x8000);
        // Is zero if all bits are 0's.
		write_NZVC(*this, CSR::Z, temp_word == 0);
		break;

	case instruction_mnemonic::CPWA:
		// The result is the decoded operand specifier plus the accumulator
        temp_word = acc + ~decoded_operand + 1;
        // Is negative if high order bit is 1.
		write_NZVC(*this, CSR::N, temp_word & 0x8000);
        // Is zero if all bits are 0's.
		write_NZVC(*this, CSR::Z, temp_word == 0);
        // There is a signed overflow iff the high order bits of the register and operand
        //are the same, and one input & the output differ in sign.
        // >> Shifts in 0's (unsigned shorts), so after shift, only high order bit remain.
		write_NZVC(*this, CSR::V, (~(acc ^ decoded_operand) & (acc ^ temp_word)) >> 15);
        // Carry out iff result is unsigned less than register or operand.
		write_NZVC(*this, CSR::C, temp_word < acc  || temp_word < decoded_operand);
		// Invert N bit if there was unsigned overflow.
		write_NZVC(*this, CSR::N, read_NZVC(*this, CSR::N) ^ read_NZVC(*this, CSR::V));
        break;
	case instruction_mnemonic::CPWX:
		// The result is the decoded operand specifier plus the accumulator
        temp_word = idx + ~decoded_operand + 1;
        // Is negative if high order bit is 1.
		write_NZVC(*this, CSR::N, temp_word & 0x8000);
        // Is zero if all bits are 0's.
		write_NZVC(*this, CSR::Z, temp_word == 0);
        // There is a signed overflow iff the high order bits of the register and operand
        //are the same, and one input & the output differ in sign.
        // >> Shifts in 0's (unsigned shorts), so after shift, only high order bit remain.
		write_NZVC(*this, CSR::V, (~(idx ^ decoded_operand) & (idx ^ temp_word)) >> 15);
        // Carry out iff result is unsigned less than register or operand.
		write_NZVC(*this, CSR::C, temp_word < idx  || temp_word < decoded_operand);
		// Invert N bit if there was unsigned overflow.
		write_NZVC(*this, CSR::N, read_NZVC(*this, CSR::N) ^ read_NZVC(*this, CSR::V));
        break;
	case instruction_mnemonic::CPBA:
		// The result is the decoded operand specifier plus the accumulator
        temp_word = acc + ~decoded_operand + 1;
		temp_word &= 0xff;
        // Is negative if high order bit is 1.
		write_NZVC(*this, CSR::N, temp_word & 0x80);
        // Is zero if all bits are 0's.
		write_NZVC(*this, CSR::Z, temp_word == 0);
		// RTL specifies that VC get 0.
		write_NZVC(*this, CSR::V, 0);
		write_NZVC(*this, CSR::C, 0);
        break;
	case instruction_mnemonic::CPBX:
		// The result is the decoded operand specifier plus the accumulator
        temp_word = idx + ~decoded_operand + 1;
		temp_word &= 0xff;
        // Is negative if high order bit is 1.
		write_NZVC(*this, CSR::N, temp_word & 0x80);
        // Is zero if all bits are 0's.
		write_NZVC(*this, CSR::Z, temp_word == 0);
		// RTL specifies that VC get 0.
		write_NZVC(*this, CSR::V, 0);
		write_NZVC(*this, CSR::C, 0);
		break;
	case instruction_mnemonic::LDWA:
		write_register(*this, Register::A, decoded_operand);
		// Is negative if high order bit is 1.
        write_NZVC(*this, CSR::N, decoded_operand & 0x8000);
         // Is zero if all bits are 0's.
        write_NZVC(*this, CSR::Z, decoded_operand == 0);
		break;
	case instruction_mnemonic::LDWX:
		write_register(*this, Register::X, decoded_operand);
		// Is negative if high order bit is 1.
        write_NZVC(*this, CSR::N, decoded_operand & 0x8000);
         // Is zero if all bits are 0's.
        write_NZVC(*this, CSR::Z, decoded_operand == 0);
		break;
	case instruction_mnemonic::LDBA: 
		write_register(*this, Register::A, decoded_operand);
		// Is negative if high order bit is 1.
        write_NZVC(*this, CSR::N, decoded_operand & 0x80);
         // Is zero if all bits are 0's.
        write_NZVC(*this, CSR::Z, decoded_operand == 0);
		break;
	case instruction_mnemonic::LDBX:
		write_register(*this, Register::X, decoded_operand);
		// Is negative if high order bit is 1.
        write_NZVC(*this, CSR::N, decoded_operand & 0x80);
         // Is zero if all bits are 0's.
        write_NZVC(*this, CSR::Z, decoded_operand == 0);
		break;
	case instruction_mnemonic::STWA:
		outcome_void = std::move(write_word(decoded_operand, read_register(*this, Register::A)));
		if(outcome_void.has_error()) return outcome_void.error().clone();
		break;
	case instruction_mnemonic::STWX:
		outcome_void = std::move(write_word(decoded_operand, read_register(*this, Register::X)));
		if(outcome_void.has_error()) return outcome_void.error().clone();
		break;
	case instruction_mnemonic::STBA: 
		outcome_void = std::move(write_byte(decoded_operand, read_register(*this, Register::A)));
		if(outcome_void.has_error()) return outcome_void.error().clone();
		break;
	case instruction_mnemonic::STBX:
		outcome_void = std::move(write_byte(decoded_operand, read_register(*this, Register::X)));
		if(outcome_void.has_error()) return outcome_void.error().clone();
		break;
	default:
		throw std::invalid_argument("Illegal instruction in nonunary decoder.");
	}
	return result<void>(OUTCOME_V2_NAMESPACE::in_place_type<void>);
}

result<uint8_t> isa::pep10::LocalProcessor::read_byte(uint16_t address) const
{

	auto locker = _owner.acquire_transaction_lock();
	auto i = _owner.read_memory(address);
	return i;
}

result<uint16_t> isa::pep10::LocalProcessor::read_word(uint16_t address) const
{
	auto locker = _owner.acquire_transaction_lock();
	auto msb = _owner.read_memory(address);
	if(msb.has_failure()) return msb.error().clone();
	auto lsb = _owner.read_memory(address+1);
	if(lsb.has_failure()) return lsb.error().clone();

	return msb.value() << 8 | lsb.value();
}

result<void> isa::pep10::LocalProcessor::write_byte(uint16_t address, uint8_t value)
{
	auto locker = _owner.acquire_transaction_lock();
	return _owner.write_memory(address, value);
}

result<void> isa::pep10::LocalProcessor::write_word(uint16_t address, uint16_t value)
{
	auto locker = _owner.acquire_transaction_lock();
	uint8_t msb = ((value & 0xff00) >> 8);
	uint8_t lsb = value & 0xff;
	auto _0 = _owner.write_memory(address, msb);
	if(_0.has_error()) return _0;
	auto _1 = _owner.write_memory(address + 1, lsb);
	if(_1.has_error()) return _1;
	else return result<void>(OUTCOME_V2_NAMESPACE::in_place_type<void>);
}

result<uint16_t> isa::pep10::LocalProcessor::decode_load_operand(const instruction_definition& instr, addressing_mode mode, uint16_t addr)
{
	using ::isa::pep10::read_register;
	result<uint8_t> outcome_byte = result<uint8_t>(OUTCOME_V2_NAMESPACE::in_place_type<uint8_t>);
	result<uint16_t> outcome_word = result<uint16_t>(OUTCOME_V2_NAMESPACE::in_place_type<uint16_t>);
	if(auto mn = instr.mnemonic; mn == instruction_mnemonic::CPBA 
		|| mn == instruction_mnemonic::CPBX
		|| mn == instruction_mnemonic::LDBA
		|| mn == instruction_mnemonic::LDBX) {
			
		switch (mode)
		{
		case addressing_mode::I: 
			return addr & 0xff;
		case addressing_mode::D: 
			outcome_byte = std::move(read_byte(addr));
			if(outcome_byte.has_error()) return outcome_byte.error().clone();
			return outcome_byte.value();
		case addressing_mode::N:
			outcome_word = std::move(read_word(addr));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			outcome_byte = std::move(read_byte(outcome_word.value()));
			if(outcome_byte.has_error()) return outcome_byte.error().clone();
			return outcome_byte.value();
		case addressing_mode::S:
			outcome_byte = std::move(read_byte(addr + read_register(*this, Register::SP)));
			if(outcome_byte.has_error()) return outcome_byte.error().clone();
			return outcome_byte.value();
		case addressing_mode::X:
			outcome_byte = std::move(read_byte(addr + read_register(*this, Register::X)));
			if(outcome_byte.has_error()) return outcome_byte.error().clone();
			return outcome_byte.value();
		case addressing_mode::SX: 
			addr += read_register(*this, Register::SP) + read_register(*this, Register::X);
			outcome_byte = std::move(read_byte(addr));
			if(outcome_byte.has_error()) return outcome_byte.error().clone();
			return outcome_byte.value();
		case addressing_mode::SF:
			outcome_word = std::move(read_word(addr + read_register(*this, Register::SP)));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			outcome_byte = std::move(read_byte(outcome_word.value()));
			if(outcome_byte.has_error()) return outcome_byte.error().clone();
			return outcome_byte.value();
		case addressing_mode::SFX:
			outcome_word = std::move(read_word(addr + read_register(*this, Register::SP)));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			outcome_byte = std::move(read_byte(outcome_word.value() + +read_register(*this, Register::X)));
			if(outcome_byte.has_error()) return outcome_byte.error().clone();
			return outcome_byte.value();
		default:
			throw std::invalid_argument("Not a valid addressing mode");
		}
	}
	else {
		switch (mode)
		{
		case addressing_mode::I: 
			return addr;
		case addressing_mode::D: 
			outcome_word = std::move(read_word(addr));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			return outcome_word.value();
		case addressing_mode::N:
			outcome_word = std::move(read_word(addr));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			outcome_word = std::move(read_word(outcome_word.value()));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			return outcome_word.value();
		case addressing_mode::S:
			outcome_word = std::move(read_word(addr + read_register(*this, Register::SP)));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			return outcome_word.value();
		case addressing_mode::X:
			outcome_word = std::move(read_word(addr + read_register(*this, Register::X)));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			return outcome_word.value();
		case addressing_mode::SX:
			addr += read_register(*this, Register::SP) + read_register(*this, Register::X);
			outcome_word = std::move(read_word(addr));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			return outcome_word.value();
		case addressing_mode::SF:
			outcome_word = std::move(read_word(addr + read_register(*this, Register::SP)));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			outcome_word = std::move(read_word(outcome_word.value()));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			return outcome_word.value();
		case addressing_mode::SFX:
			outcome_word = std::move(read_word(addr + read_register(*this, Register::SP)));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			outcome_word = std::move(read_word(outcome_word.value() + +read_register(*this, Register::X)));
			if(outcome_word.has_error()) return outcome_word.error().clone();
			return outcome_word.value();
		default:
			throw std::invalid_argument("Not a valid addressing mode");
		}
	}
	throw std::invalid_argument("Not a valid addressing mode");
}

result<uint16_t> isa::pep10::LocalProcessor::decode_store_operand(const instruction_definition& instr, addressing_mode mode, uint16_t addr)
{
	using ::isa::pep10::read_register;
	result<uint16_t> outcome_word = result<uint16_t>(OUTCOME_V2_NAMESPACE::in_place_type<uint16_t>);
	switch (mode)
	{
	case addressing_mode::D: 
		return addr;
	case addressing_mode::N:
		return read_word(addr);
	case addressing_mode::S:
		return addr + read_register(*this, Register::SP);
	case addressing_mode::X:
		return addr + read_register(*this, Register::X);
	case addressing_mode::SX:
		addr += read_register(*this, Register::SP) + read_register(*this, Register::X);
		return addr;
	case addressing_mode::SF:
		return read_word(addr + read_register(*this, Register::SP));
	case addressing_mode::SFX:
		outcome_word = std::move(read_word(addr + read_register(*this, Register::SP)));
		if(outcome_word.has_error()) return outcome_word.error().clone();
		return outcome_word.value() + read_register(*this, Register::X);
	default:
		throw std::invalid_argument("Not a valid addressing mode");
	}
	// Not reachable, but necessary to silence compiler warning
	throw std::invalid_argument("Unreachable");
}

uint16_t isa::pep10::read_register(const LocalProcessor& proc, Register reg) 
{
	return proc.read_register(static_cast<uint8_t>(reg));
}

void isa::pep10::write_register(LocalProcessor& proc, Register reg, uint16_t value)
{
	proc.write_register(static_cast<uint8_t>(reg), value);
}

bool isa::pep10::read_NZVC(const LocalProcessor& proc, CSR reg)
{
	return proc.read_csr(static_cast<uint8_t>(reg));
}

void isa::pep10::write_NZVC(LocalProcessor& proc, CSR reg, bool value)
{
	proc.write_csr(static_cast<uint8_t>(reg), value);
}

uint8_t isa::pep10::read_packed_NZVC(const LocalProcessor& proc)
{
	uint8_t NZVC = 0;
	NZVC |= read_NZVC(proc, CSR::N) << 3;
	NZVC |= read_NZVC(proc, CSR::Z) << 2;
	NZVC |= read_NZVC(proc, CSR::V) << 1;
	NZVC |= read_NZVC(proc, CSR::C) << 0;
	return NZVC;
}

void isa::pep10::write_packed_NZVC(LocalProcessor& proc, uint8_t packed)
{
	write_NZVC(proc, CSR::N, packed & 0b1000);
	write_NZVC(proc, CSR::Z, packed & 0b0100);
	write_NZVC(proc, CSR::V, packed & 0b0010);
	write_NZVC(proc, CSR::C, packed & 0b0001);
}
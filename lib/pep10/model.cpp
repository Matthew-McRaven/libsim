#include "model.hpp"

#include <iostream>
#include <stdexcept>

#include "version.hpp"
#include "interface.hpp"
#include "defs.hpp"
#include "instruction.hpp"
using namespace pep10::isa;

isa_processor::isa_processor(pep10::isa::interface<uint16_t, uint16_t, bool> iface): _iface(iface)
{

}

int isa_processor::step()
{
	_iface.begin_instruction();

	// Load PC.
	uint16_t pc = _iface.read_reg((uint8_t) Registers::PC); 

	// Load instruction spec from memory.
	uint8_t is = read_byte(pc, false);
	write_reg(Registers::IS, is);
	_iface.write_reg((uint8_t) Registers::IS, is);
	
	// Increment PC
	write_reg(Registers::PC, ++pc);

	if(is_opcode_unary(is)) {
		unary_dispatch(is);
	} else {
		// Load operand spec from memory.
		uint16_t os = read_word(pc, false);
		write_reg(Registers::OS, os);

		// Increment PC
		write_reg(Registers::PC, pc+2);

		nonunary_dispatch(is, os);
	}
}

void isa_processor::init()
{
	throw std::invalid_argument("Not a valid addressing mode");
}

void isa_processor::debug(bool)
{
	throw std::invalid_argument("Not a valid addressing mode");
}

void isa_processor::clear()
{
	throw std::invalid_argument("Not a valid addressing mode");
}

void isa_processor::unary_dispatch(uint8_t is)
{
	auto [instr, addr] = pep10::isa::definition.riproll[is];
	uint16_t temp, sp, acc, idx, vector_value;
    uint8_t temp_byte;
	sp = read_reg(Registers::SP);
	acc = read_reg(Registers::A);
	idx = read_reg(Registers::X);
    switch(instr->mnemonic) {
	case instruction_mnemonic::RET:
		temp = read_word(sp, false);
		write_reg(Registers::PC, temp);
		write_reg(Registers::SP, sp+2);
		break;

	case instruction_mnemonic::SRET:
		// Only perform single byte read as status bits do not span full word.
		temp_byte = read_byte(sp, false);
		// Function will automatically mask out bits that don't matter
		set_NZVC(temp_byte);
		
        temp = read_word(sp + 1, false);
		write_reg(Registers::A, temp);
        temp = read_word(sp + 3, false);
		write_reg(Registers::X, temp);
        temp = read_word(sp + 5, false);
		write_reg(Registers::PC, temp);
        temp = read_word(sp + 7, false);
		write_reg(Registers::SP, temp);
        break;

	case instruction_mnemonic::MOVSPA:
		write_reg(Registers::A, sp);
		break;
	case instruction_mnemonic::MOVASP:break;
		write_reg(Registers::SP, acc);
		break;

	case instruction_mnemonic::MOVFLGA:
		write_reg(Registers::A, get_packed_NZVC());
		break;
	case instruction_mnemonic::MOVAFLG:
		set_NZVC(acc);
		break;

	case instruction_mnemonic::MOVTA:
		temp = read_reg(Registers::TR);
		write_reg(Registers::A, temp);
		break;
	case instruction_mnemonic::NOP:
		break;

	case instruction_mnemonic::NOTA:
		acc = ~acc;
		write_reg(Registers::A, acc);
		set_NZVC(CSR::N, acc & 0x8000);
		set_NZVC(CSR::Z, acc == 0x0);
		break;
	case instruction_mnemonic::NOTX:break;
		idx = ~idx;
		write_reg(Registers::X, idx);
		set_NZVC(CSR::N, idx & 0x8000);
		set_NZVC(CSR::Z, idx == 0x0);
		break;

	case instruction_mnemonic::NEGA:
		acc = ~acc + 1;
		write_reg(Registers::A, acc);
		set_NZVC(CSR::N, acc & 0x8000);
		set_NZVC(CSR::Z, acc == 0x0);
		set_NZVC(CSR::V, acc == 0x8000);
		break;
	case instruction_mnemonic::NEGX:
		idx = ~idx + 1;
		write_reg(Registers::X, idx);
		set_NZVC(CSR::N, idx & 0x8000);
		set_NZVC(CSR::Z, idx == 0x0);
		set_NZVC(CSR::V, idx == 0x8000);
		break;

	case instruction_mnemonic::ASLA:
		// Store in temp, because we need acc for status bit computation.
		temp = static_cast<uint16_t>(acc<<1);
		write_reg(Registers::A, temp);
		// Is negative if high order bit is 1.
        set_NZVC(CSR::N, temp & 0x8000);
         // Is zero if all bits are 0's.
        set_NZVC(CSR::Z, temp == 0);
        // Signed overflow occurs when the starting & ending values of the high order bit differ (a xor temp == 1).
        // Then shift the result over by 15 places to only keep high order bit (which is the sign).
        set_NZVC(CSR::V, (acc ^ temp) >> 15);
        // Carry out if register starts with high order 1.
        set_NZVC(CSR::C, acc & 0x8000);
		break;
	case instruction_mnemonic::ASLX:
		// Store in temp, because we need acc for status bit computation.
		temp = static_cast<uint16_t>(idx<<1);
		write_reg(Registers::X, temp);
		// Is negative if high order bit is 1.
        set_NZVC(CSR::N, temp & 0x8000);
         // Is zero if all bits are 0's.
        set_NZVC(CSR::Z, temp == 0);
        // Signed overflow occurs when the starting & ending values of the high order bit differ (a xor temp == 1).
        // Then shift the result over by 15 places to only keep high order bit (which is the sign).
        set_NZVC(CSR::V, (idx ^ temp) >> 15);
        // Carry out if register starts with high order 1.
        set_NZVC(CSR::C, idx & 0x8000);
		break;

	case instruction_mnemonic::ASRA:
		// Shift all bits to the right by 1 position. Since using unsigned shift, must explicitly
        // perform sign extension by hand.
        temp = static_cast<uint16_t>(acc >> 1 | ((acc & 0x8000) ? 1<<15 : 0));
		write_reg(Registers::A, temp);
		// Is negative if high order bit is 1.
        set_NZVC(CSR::N, temp & 0x8000);
         // Is zero if all bits are 0's.
        set_NZVC(CSR::Z, temp == 0);
        // Carry out if register starts with low order 1.
        set_NZVC(CSR::C, acc & 0x1);
        break;
	case instruction_mnemonic::ASRX:
		// Shift all bits to the right by 1 position. Since using unsigned shift, must explicitly
        // perform sign extension by hand.
        temp = static_cast<uint16_t>(idx >> 1 | ((idx & 0x8000) ? 1<<15 : 0));
        write_reg(Registers::X, temp);                                                       
		// Is negative if high order bit is 1.
        set_NZVC(CSR::N, temp & 0x8000);
         // Is zero if all bits are 0's.
        set_NZVC(CSR::Z, temp == 0);
        // Carry out if register starts with low order 1.
        set_NZVC(CSR::C, idx & 0x1);
        break;

	case instruction_mnemonic::ROLA:
		// Shift the carry in to low order bit.
		temp = static_cast<uint16_t> (acc << 1 | (get_NZVC(CSR::C) ? 1 : 0));
        write_reg(Registers::A, temp);
		// Carry out if register starts with high order 1.                       
		set_NZVC(CSR::C, acc & 0x8000);
        break;
	case instruction_mnemonic::ROLX:
		// Shift the carry in to low order bit.
		temp = static_cast<uint16_t> (idx << 1 | (get_NZVC(CSR::C) ? 1 : 0));
        write_reg(Registers::X, temp);      
		// Carry out if register starts with high order 1.                       
		set_NZVC(CSR::C, idx & 0x8000);
        break;

	case instruction_mnemonic::RORA:
		// Shift the carry in to high order bit.
		temp = static_cast<uint16_t> (acc >> 1 | (get_NZVC(CSR::C) ? 1<<15 : 0));
        write_reg(Registers::A, temp);      
		// Carry out if register starts with low order 1.                       
		set_NZVC(CSR::C, acc & 0x1);
        break;
	case instruction_mnemonic::RORX:
		// Shift the carry in to high order bit.
		temp = static_cast<uint16_t> (idx >> 1 | (get_NZVC(CSR::C) ? 1<<15 : 0));
        write_reg(Registers::X, temp);      
		// Carry out if register starts with low order 1.                       
		set_NZVC(CSR::C, idx & 0x1);
        break;
	case instruction_mnemonic::SCALL:
		[[fallthrough]]
	case instruction_mnemonic::USCALL:
		vector_value = _iface.addr_from_vector(memory_vectors::SYSTEM_STACK);
		temp = read_word(vector_value, true);
		// Writes IS to mem[T-1].
		write_byte(temp -1, read_reg(Registers::IS), false);
        // Writes SP to mem[T-2], mem[T-3].
		write_word(temp - 3, read_reg(Registers::SP), false);
        // Writes to mem[T-4], mem[T-5].
		write_word(temp - 5, read_reg(Registers::PC), false);
        // Writes to mem[T-6], mem[T-7].
		write_word(temp - 7, read_reg(Registers::X), false);
        // Writes to mem[T-8], mem[T-9].
		write_word(temp - 9, read_reg(Registers::A), false);
        // Writes NZVC to mem[T-10].
		write_byte(temp - 10, get_packed_NZVC(), false);
		write_reg(Registers::SP, temp - 10);
		vector_value = _iface.addr_from_vector(memory_vectors::TRAP);
		write_reg(Registers::PC, read_word(vector_value, false));    
		break;

	}
}
void isa_processor::nonunary_dispatch(uint8_t is, uint16_t os)
{
	auto [instr, addr] = definition.riproll[is];
	uint16_t temp, sp, acc, idx, vector_value;
    uint8_t temp_byte;
	sp = read_reg(Registers::SP);
	acc = read_reg(Registers::A);
	idx = read_reg(Registers::X);

	uint16_t decoded_operand = 0;
	if( is_store(is)) decoded_operand = decode_store_operand(instr, addr, os);
	else decoded_operand = decode_load_operand(instr, addr, os);

	switch (instr->mnemonic) {
	case instruction_mnemonic::BR:
		write_reg(Registers::PC, decoded_operand);
		break;
	case instruction_mnemonic::BRLE:
		if(get_NZVC(CSR::N) || get_NZVC(CSR::Z)) {
			write_reg(Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRLT:
		if(get_NZVC(CSR::N)) {
			write_reg(Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BREQ:
		if(get_NZVC(CSR::Z)) {
			write_reg(Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRNE:
		if(!get_NZVC(CSR::Z)) {
			write_reg(Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRGE:
		if(!get_NZVC(CSR::N)) {
			write_reg(Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRGT:
		if(!get_NZVC(CSR::N) && !get_NZVC(CSR::Z) ) {
			write_reg(Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRV:
		if(get_NZVC(CSR::V)) {
			write_reg(Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRC:
		if(get_NZVC(CSR::C)) {
			write_reg(Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::CALL:
        sp -= 2;
		write_word(sp, read_reg(Registers::PC), false);
		// Load decoded operand as new PC.
		write_reg(Registers::PC, decoded_operand);
		// Update SP with new value
		write_reg(Registers::SP, sp);
        break;

	case instruction_mnemonic::LDWT:
		write_reg(Registers::TR, decoded_operand);
		break;
	case instruction_mnemonic::LDWA:
		write_reg(Registers::A, decoded_operand);
		// Is negative if high order bit is 1.
        set_NZVC(CSR::N, decoded_operand & 0x8000);
         // Is zero if all bits are 0's.
        set_NZVC(CSR::Z, decoded_operand == 0);
		break;
	case instruction_mnemonic::LDWX:
		write_reg(Registers::X, decoded_operand);
		// Is negative if high order bit is 1.
        set_NZVC(CSR::N, decoded_operand & 0x8000);
         // Is zero if all bits are 0's.
        set_NZVC(CSR::Z, decoded_operand == 0);
		break;
	case instruction_mnemonic::LDBA: 
		write_reg(Registers::A, decoded_operand);
		// Is negative if high order bit is 1.
        set_NZVC(CSR::N, decoded_operand & 0x80);
         // Is zero if all bits are 0's.
        set_NZVC(CSR::Z, decoded_operand == 0);
		break;
	case instruction_mnemonic::LDBX:
		write_reg(Registers::X, decoded_operand);
		// Is negative if high order bit is 1.
        set_NZVC(CSR::N, decoded_operand & 0x80);
         // Is zero if all bits are 0's.
        set_NZVC(CSR::Z, decoded_operand == 0);
		break;
	case instruction_mnemonic::STWA:
		write_word(decoded_operand, read_reg(Registers::A), false);
		break;
	case instruction_mnemonic::STWX:
		write_word(decoded_operand, read_reg(Registers::X), false);
		break;
	case instruction_mnemonic::STBA: 
		write_byte(decoded_operand, read_reg(Registers::A), false);
		break;
	case instruction_mnemonic::STBX:
		write_byte(decoded_operand, read_reg(Registers::X), false);
		break;

	case instruction_mnemonic::CPWA:
		// The result is the decoded operand specifier plus the accumulator
        temp = acc + ~decoded_operand + 1;
        // Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x8000);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
        // There is a signed overflow iff the high order bits of the register and operand
        //are the same, and one input & the output differ in sign.
        // >> Shifts in 0's (unsigned shorts), so after shift, only high order bit remain.
		set_NZVC(CSR::V, (~(acc ^ decoded_operand) & (acc ^ temp)) >> 15);
        // Carry out iff result is unsigned less than register or operand.
		set_NZVC(CSR::C, temp < acc  || temp < decoded_operand);
		// Invert N bit if there was unsigned overflow.
		set_NZVC(CSR::N, get_NZVC(CSR::N) ^ get_NZVC(CSR::V));
        break;
	case instruction_mnemonic::CPWX:
		// The result is the decoded operand specifier plus the accumulator
        temp = idx + ~decoded_operand + 1;
        // Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x8000);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
        // There is a signed overflow iff the high order bits of the register and operand
        //are the same, and one input & the output differ in sign.
        // >> Shifts in 0's (unsigned shorts), so after shift, only high order bit remain.
		set_NZVC(CSR::V, (~(idx ^ decoded_operand) & (idx ^ temp)) >> 15);
        // Carry out iff result is unsigned less than register or operand.
		set_NZVC(CSR::C, temp < idx  || temp < decoded_operand);
		// Invert N bit if there was unsigned overflow.
		set_NZVC(CSR::N, get_NZVC(CSR::N) ^ get_NZVC(CSR::V));
        break;
	case instruction_mnemonic::CPBA:
		// The result is the decoded operand specifier plus the accumulator
        temp = acc + ~decoded_operand + 1;
		temp &= 0xff;
        // Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x80);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
		// RTL specifies that VC get 0.
		set_NZVC(CSR::V, 0);
		set_NZVC(CSR::C, 0);
        break;
	case instruction_mnemonic::CPBX:
		// The result is the decoded operand specifier plus the accumulator
        temp = idx + ~decoded_operand + 1;
		temp &= 0xff;
        // Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x80);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
		// RTL specifies that VC get 0.
		set_NZVC(CSR::V, 0);
		set_NZVC(CSR::C, 0);
		break;

	case instruction_mnemonic::ADDA:
		// The result is the decoded operand specifier plus the accumulator
        temp = acc + decoded_operand;
		write_reg(Registers::A, temp);
        // Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x8000);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
        // There is a signed overflow iff the high order bits of the register and operand
        //are the same, and one input & the output differ in sign.
        // >> Shifts in 0's (unsigned shorts), so after shift, only high order bit remain.
		set_NZVC(CSR::V, (~(acc ^ decoded_operand) & (acc ^ temp)) >> 15);
        // Carry out iff result is unsigned less than register or operand.
		set_NZVC(CSR::C, temp < acc  || temp < decoded_operand);
        break;
	case instruction_mnemonic::ADDX:
		// The result is the decoded operand specifier plus the accumulator
        temp = idx + decoded_operand;
		write_reg(Registers::X, temp);
        // Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x8000);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
        // There is a signed overflow iff the high order bits of the register and operand
        //are the same, and one input & the output differ in sign.
        // >> Shifts in 0's (unsigned shorts), so after shift, only high order bit remain.
		set_NZVC(CSR::V, (~(idx ^ decoded_operand) & (idx ^ temp)) >> 15);
        // Carry out iff result is unsigned less than register or operand.
		set_NZVC(CSR::C, temp < idx  || temp < decoded_operand);
        break;
	case instruction_mnemonic::SUBA:
		// The result is the decoded operand specifier plus the accumulator
        temp = acc + ~decoded_operand + 1;
		write_reg(Registers::A, temp);
        // Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x8000);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
        // There is a signed overflow iff the high order bits of the register and operand
        //are the same, and one input & the output differ in sign.
        // >> Shifts in 0's (unsigned shorts), so after shift, only high order bit remain.
		set_NZVC(CSR::V, (~(acc ^ decoded_operand) & (acc ^ temp)) >> 15);
        // Carry out iff result is unsigned less than register or operand.
		set_NZVC(CSR::C, temp < acc  || temp < decoded_operand);
        break;
	case instruction_mnemonic::SUBX:
		// The result is the decoded operand specifier plus the accumulator
        temp = idx + ~decoded_operand + 1;
		write_reg(Registers::X, temp);
        // Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x8000);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
        // There is a signed overflow iff the high order bits of the register and operand
        //are the same, and one input & the output differ in sign.
        // >> Shifts in 0's (unsigned shorts), so after shift, only high order bit remain.
		set_NZVC(CSR::V, (~(idx ^ decoded_operand) & (idx ^ temp)) >> 15);
        // Carry out iff result is unsigned less than register or operand.
		set_NZVC(CSR::C, temp < idx  || temp < decoded_operand);
        break;

	case instruction_mnemonic::ANDA:
		temp = acc & decoded_operand;
		write_reg(Registers::A, temp);
		// Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x8000);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
		break;
	case instruction_mnemonic::ANDX:
		temp = idx & decoded_operand;
		write_reg(Registers::X, temp);
		// Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x8000);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
		break;
	case instruction_mnemonic::ORA:
		temp = acc | decoded_operand;
		write_reg(Registers::A, temp);
		// Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x8000);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
		break;
	case instruction_mnemonic::ORX:
		temp = idx | decoded_operand;
		write_reg(Registers::X, temp);
		// Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x8000);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
		break;
	case instruction_mnemonic::XORA:
		temp = acc ^ decoded_operand;
		write_reg(Registers::A, temp);
		// Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x8000);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
		break;
	case instruction_mnemonic::XORX:
		temp = idx ^ decoded_operand;
		write_reg(Registers::X, temp);
		// Is negative if high order bit is 1.
		set_NZVC(CSR::N, temp & 0x8000);
        // Is zero if all bits are 0's.
		set_NZVC(CSR::Z, temp == 0);
		break;
	
	case instruction_mnemonic::ADDSP:
		write_reg(Registers::SP, sp+decoded_operand);
		break;
	case instruction_mnemonic::SUBSP:
		write_reg(Registers::SP, sp-decoded_operand);
		break;
	
	}
}



uint16_t isa_processor::decode_load_operand(const instruction_definition<uint8_t>* instr, addressing_mode mode, uint16_t addr) const
{

	if(auto mn = instr->mnemonic; mn == instruction_mnemonic::CPBA 
		|| mn == instruction_mnemonic::CPBX
		|| mn == instruction_mnemonic::LDBA
		|| mn == instruction_mnemonic::LDBX) {
			
		switch (mode)
		{
		case addressing_mode::I: 
			return addr & 0xff;
		case addressing_mode::D: 
			return read_byte(addr, false);
		case addressing_mode::N:
			addr = read_word(addr, false);
			return read_byte(addr, false);
		case addressing_mode::S:
			return read_byte(addr + read_reg(Registers::SP), false);
		case addressing_mode::X:
			return read_byte(addr + read_reg(Registers::X), false);
		case addressing_mode::SX: break;
			addr += read_reg(Registers::SP) + read_reg(Registers::X);
			return read_byte(addr, false);
		case addressing_mode::SF:
			addr = read_word(addr + read_reg(Registers::SP), false);
			return read_byte(addr, false);
		case addressing_mode::SFX:
			addr = read_word(addr + read_reg(Registers::SP), false);
			return read_byte(addr + read_reg(Registers::X), false);
		}
	}
	else {
		switch (mode)
		{
		case addressing_mode::I: 
			return addr;
		case addressing_mode::D: 
			return read_word(addr, false);
		case addressing_mode::N:
			addr = read_word(addr, false);
			return read_word(addr, false);
		case addressing_mode::S:
			return read_word(addr + read_reg(Registers::SP), false);
		case addressing_mode::X:
			return read_word(addr + read_reg(Registers::X), false);
		case addressing_mode::SX: break;
			addr += read_reg(Registers::SP) + read_reg(Registers::X);
			return read_word(addr, false);
		case addressing_mode::SF:
			addr = read_word(addr + read_reg(Registers::SP), false);
			return read_word(addr, false);
		case addressing_mode::SFX:
			addr = read_word(addr + read_reg(Registers::SP), false);
			return read_word(addr + read_reg(Registers::X), false);
		}
	}
	throw std::invalid_argument("Not a valid addressing mode");
}

uint16_t isa_processor::decode_store_operand(const instruction_definition<uint8_t>* instr, addressing_mode mode, uint16_t addr) const
{
	switch (mode)
	{
	case addressing_mode::D: 
		return addr;
	case addressing_mode::N:
		return read_word(addr, false);
	case addressing_mode::S:
		return addr + read_reg(Registers::SP);
	case addressing_mode::X:
		return addr + read_reg(Registers::X);
	case addressing_mode::SX: break;
		addr += read_reg(Registers::SP) + read_reg(Registers::X);
		return addr;
	case addressing_mode::SF:
		return read_word(addr + read_reg(Registers::SP), false);
	case addressing_mode::SFX:
		addr = read_word(addr + read_reg(Registers::SP), false);
		return addr + read_reg(Registers::X);
	}
	throw std::invalid_argument("Not a valid addressing mode");
}

uint16_t isa_processor::read_reg(Registers reg) const
{
	return _iface.read_reg(static_cast<uint8_t>(reg));
}

void isa_processor::write_reg(Registers reg, uint16_t value)
{
	_iface.write_reg(static_cast<uint8_t>(reg), value);
}

uint8_t isa_processor::read_byte(uint16_t address, bool allow_side_effects) const
{
	return _iface.read_mem_byte(address, allow_side_effects);
}

uint16_t isa_processor::read_word(uint16_t address, bool allow_side_effects) const
{
	auto msb = _iface.read_mem_byte(address, allow_side_effects);
	auto lsb = _iface.read_mem_byte(address+1, allow_side_effects);
	return msb << 8 | lsb;
}

void isa_processor::write_byte(uint16_t address, uint8_t value, bool allow_side_effects) const
{
	return _iface.write_mem_byte(address, value, allow_side_effects);
}

void isa_processor::write_word(uint16_t address, uint16_t value, bool allow_side_effects)
{
	uint8_t msb = ((value & 0xff00) >> 8);
	uint8_t lsb = value & 0xff;
	write_byte(address, msb, allow_side_effects);
	write_byte(address + 1, lsb, allow_side_effects);
}

void isa_processor::set_NZVC(uint8_t packed)
{
	set_NZVC(CSR::N, packed & 0b1000);
	set_NZVC(CSR::Z, packed & 0b0100);
	set_NZVC(CSR::V, packed & 0b0010);
	set_NZVC(CSR::C, packed & 0b0001);
}

void isa_processor::set_NZVC(CSR reg, bool value)
{
	_iface.write_csr(static_cast<uint8_t>(reg), value);
}

uint8_t isa_processor::get_packed_NZVC() const
{
	uint8_t NZVC = 0;
	NZVC |= get_NZVC(CSR::N) << 3;
	NZVC |= get_NZVC(CSR::Z) << 2;
	NZVC |= get_NZVC(CSR::V) << 1;
	NZVC |= get_NZVC(CSR::C) << 0;
	return NZVC;
}

bool isa_processor::get_NZVC(CSR reg) const
{
	return _iface.read_csr(static_cast<uint8_t>(reg));
}
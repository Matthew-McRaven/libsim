#include "model.hpp"

#include <iostream>

#include "version.hpp"
#include "interface.hpp"
#include "defs.hpp"
#include "instruction.hpp"

int main() {
	std::cout << RTL_Model_VERSION_MAJOR << RTL_Model_VERSION_MINOR << std::endl;
}

isa_processor::isa_processor(interface<uint16_t, uint16_t, bool> iface): _iface(iface)
{

}
int isa_processor::step()
{
	_iface.begin_instruction();

	// Load PC.
	uint16_t pc = _iface.read_reg((uint8_t) Registers::PC); 

	// Load instruction spec from memory.
	_iface.begin_transaction();
	uint8_t is = _iface.read_mem_byte(pc, false);
	_iface.end_transaction();
	_iface.write_reg((uint8_t) Registers::IS, is);
	
	// Increment PC
	_iface.write_reg((uint8_t) Registers::PC, ++pc);

	if(is_opcode_unary(is)) {
		unary_dispatch(is);
	} else {
		// Load operand spec from memory.
		_iface.begin_transaction();
		uint16_t os = _iface.read_mem_byte(pc, false) << 8 | _iface.read_mem_byte(pc+1, false);
		_iface.end_transaction();
		_iface.write_reg((uint8_t) Registers::OS, os);

		// Increment PC
		_iface.write_reg((uint8_t) Registers::PC, pc+2);

		nonunary_dispatch(is, os);
	}
}

void isa_processor::init()
{
	
}

void isa_processor::debug(bool)
{

}

void isa_processor::clear()
{

}

void isa_processor::unary_dispatch(uint8_t is)
{
	auto [instr, addr] = isa.riproll[is];
	uint16_t temp, sp, acc, idx, vector_value;
    uint8_t temp_byte;
    sp = _iface.read_reg((uint8_t) Registers::SP);
    acc = _iface.read_reg((uint8_t) Registers::A);
    idx = _iface.read_reg((uint8_t) Registers::X);

	switch(instr->mnemonic) {
	case instruction_mnemonic::RET:
		temp = read_word(sp, false);
		_iface.write_reg((uint8_t) Registers::PC, temp);
		_iface.write_reg((uint8_t) Registers::SP, sp+2);
        break;

	case instruction_mnemonic::SRET:
		// Only perform single byte read as status bits do not span full word.
        temp_byte = _iface.read_mem_byte(sp, false);
        // Function will automatically mask out bits that don't matter
		set_NZVC(temp_byte);
        temp = read_word(sp + 1, false);
		_iface.write_reg((uint8_t) Registers::A, temp);
        temp = read_word(sp + 3, false);
        _iface.write_reg((uint8_t) Registers::X, temp);
        temp = read_word(sp + 5, false);
        _iface.write_reg((uint8_t) Registers::PC, temp);
        temp = read_word(sp + 7, false);
        _iface.write_reg((uint8_t) Registers::SP, temp);
        break;

	case instruction_mnemonic::MOVSPA:
		_iface.write_reg((uint8_t) Registers::A, sp);
		break;
	case instruction_mnemonic::MOVASP:break;
		_iface.write_reg((uint8_t) Registers::SP, acc);
		break;

	case instruction_mnemonic::MOVFLGA:
		_iface.write_reg((uint8_t) Registers::A, get_packed_NZVC());
		break;
	case instruction_mnemonic::MOVAFLG:
		set_NZVC(acc);
		break;

	case instruction_mnemonic::MOVTA:
		temp = _iface.read_reg((uint8_t) Registers::TR);
		_iface.write_reg((uint8_t) Registers::A, temp);
		break;
	case instruction_mnemonic::NOP:
		break;

	case instruction_mnemonic::NOTA:
		acc = ~acc;
		_iface.write_reg((uint8_t) Registers::A, acc);
		set_NZVC(CSR::N, acc & 0x8000);
		set_NZVC(CSR::Z, acc == 0x0);
		break;
	case instruction_mnemonic::NOTX:break;
		idx = ~idx;
		_iface.write_reg((uint8_t) Registers::X, idx);
		set_NZVC(CSR::N, idx & 0x8000);
		set_NZVC(CSR::Z, idx == 0x0);
		break;

	case instruction_mnemonic::NEGA:
		acc = ~acc;
		_iface.write_reg((uint8_t) Registers::A, acc);
		set_NZVC(CSR::N, acc & 0x8000);
		set_NZVC(CSR::Z, acc == 0x0);
		set_NZVC(CSR::V, acc == 0x8000);
		break;
	case instruction_mnemonic::NEGX:
		idx = ~idx;
		_iface.write_reg((uint8_t) Registers::X, idx);
		set_NZVC(CSR::N, idx & 0x8000);
		set_NZVC(CSR::Z, idx == 0x0);
		set_NZVC(CSR::V, idx == 0x8000);
		break;

	case instruction_mnemonic::ASLA:
		// Store in temp, because we need acc for status bit computation.
		temp = static_cast<uint16_t>(acc<<1);
		_iface.write_reg((uint8_t) Registers::A, temp);
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
		_iface.write_reg((uint8_t) Registers::X, temp);
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
        _iface.write_reg((uint8_t) Registers::A, temp);                                                         
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
        _iface.write_reg((uint8_t) Registers::X, temp);                                                         
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
        _iface.write_reg((uint8_t) Registers::A, temp);       
		// Carry out if register starts with high order 1.                       
		set_NZVC(CSR::C, acc & 0x8000);
        break;
	case instruction_mnemonic::ROLX:
		// Shift the carry in to low order bit.
		temp = static_cast<uint16_t> (idx << 1 | (get_NZVC(CSR::C) ? 1 : 0));
        _iface.write_reg((uint8_t) Registers::X, temp);       
		// Carry out if register starts with high order 1.                       
		set_NZVC(CSR::C, idx & 0x8000);
        break;

	case instruction_mnemonic::RORA:
		// Shift the carry in to high order bit.
		temp = static_cast<uint16_t> (acc >> 1 | (get_NZVC(CSR::C) ? 1<<15 : 0));
        _iface.write_reg((uint8_t) Registers::A, temp);       
		// Carry out if register starts with low order 1.                       
		set_NZVC(CSR::C, acc & 0x1);
        break;
	case instruction_mnemonic::RORX:
		// Shift the carry in to high order bit.
		temp = static_cast<uint16_t> (idx >> 1 | (get_NZVC(CSR::C) ? 1<<15 : 0));
        _iface.write_reg((uint8_t) Registers::X, temp);       
		// Carry out if register starts with low order 1.                       
		set_NZVC(CSR::C, idx & 0x1);
        break;
	case instruction_mnemonic::SCALL:
		[[fallthrough]]
	case instruction_mnemonic::USCALL:
		vector_value = _iface.addr_from_vector(memory_vectors::SYSTEM_STACK);
		temp = read_word(vector_value, true);
		// Writes IS to mem[T-1].
		_iface.write_mem_byte(temp - 1, _iface.read_reg((uint8_t)Registers::IS), false);
        // Writes SP to mem[T-2], mem[T-3].
		write_word(temp - 3, _iface.read_reg((uint8_t) Registers::SP), false);
        // Writes to mem[T-4], mem[T-5].
		write_word(temp - 5, _iface.read_reg((uint8_t) Registers::PC), false);
        // Writes to mem[T-6], mem[T-7].
		write_word(temp - 7, _iface.read_reg((uint8_t) Registers::X), false);
        // Writes to mem[T-8], mem[T-9].
		write_word(temp - 9, _iface.read_reg((uint8_t) Registers::A), false);
        // Writes NZVC to mem[T-10].
		_iface.write_mem_byte(temp - 10, get_packed_NZVC(), false);
		_iface.write_reg((uint8_t) Registers::SP, temp - 10);
		vector_value = _iface.addr_from_vector(memory_vectors::TRAP);    
        _iface.write_reg((uint8_t) Registers::PC, read_word(vector_value, false));
		break;

	}
}
void isa_processor::nonunary_dispatch(uint8_t is, uint16_t os)
{
	auto [instr, addr] = isa.riproll[is];
	//auto inst = std::get<0>(isa.riproll[is]);
	//auto addr = std::get<1>(isa.riproll[is]);

	uint16_t decoded_operand = 0;
	if( is_store(is)) decoded_operand = decode_store_operand(addr, os);
	else decoded_operand = decode_load_operand(addr, os);

	switch (instr->mnemonic) {
	case instruction_mnemonic::BR:
		_iface.write_reg((uint8_t) Registers::PC, decoded_operand);
		break;
	case instruction_mnemonic::BRLE:
		if(get_NZVC(CSR::N) || get_NZVC(CSR::Z)) {
			_iface.write_reg((uint8_t) Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRLT:
		if(get_NZVC(CSR::N))) {
			_iface.write_reg((uint8_t) Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BREQ:
		if(get_NZVC(CSR::Z)) {
			_iface.write_reg((uint8_t) Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRNE:
		if(!get_NZVC(CSR::Z)) {
			_iface.write_reg((uint8_t) Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRGE:
		if(!get_NZVC(CSR::N)) {
			_iface.write_reg((uint8_t) Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRGT:
		if(!get_NZVC(CSR::N) && !get_NZVC(CSR::Z) ) {
			_iface.write_reg((uint8_t) Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRV:
		if(get_NZVC(CSR::V)) {
			_iface.write_reg((uint8_t) Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::BRC:
		if(get_NZVC(CSR::C)) {
			_iface.write_reg((uint8_t) Registers::PC, decoded_operand);
		}
		break;
	case instruction_mnemonic::CALL: break;

	case instruction_mnemonic::LDWT: break;
	case instruction_mnemonic::LDWA: break;
	case instruction_mnemonic::LDWX: break;
	case instruction_mnemonic::LDBA: break;
	case instruction_mnemonic::LDBX: break;
	case instruction_mnemonic::STWA: break;
	case instruction_mnemonic::STWX: break;
	case instruction_mnemonic::STBA: break;
	case instruction_mnemonic::STBX: break;

	case instruction_mnemonic::CPWA: break;
	case instruction_mnemonic::CPWX: break;
	case instruction_mnemonic::CPBA: break;
	case instruction_mnemonic::CPBX: break;

	case instruction_mnemonic::ADDA: break;
	case instruction_mnemonic::ADDX: break;
	case instruction_mnemonic::SUBA: break;
	case instruction_mnemonic::SUBX: break;
	case instruction_mnemonic::ANDA: break;
	case instruction_mnemonic::ANDX: break;
	case instruction_mnemonic::ORA: break;
	case instruction_mnemonic::ORX: break;
	case instruction_mnemonic::XORA: break;
	case instruction_mnemonic::XORX: break;
	
	case instruction_mnemonic::ADDSP: break;
	case instruction_mnemonic::SUBSP: break;
	
	
	}
}

uint16_t isa_processor::decode_load_operand(addressing_mode mode, uint16_t addr) const
{
	return 0;
}
uint16_t isa_processor::decode_store_operand(addressing_mode mode, uint16_t addr) const
{
	return 0;
}

uint16_t isa_processor::read_word(uint16_t address, bool) const
{

}
void isa_processor::set_NZVC(uint8_t packed)
{

}

void isa_processor::write_word(uint16_t address, uint16_t value, bool)
{

}

void isa_processor::set_NZVC(CSR reg, bool value)
{

}

uint8_t isa_processor::get_packed_NZVC() const
{

}

bool isa_processor::get_NZVC(CSR reg) const
{

}
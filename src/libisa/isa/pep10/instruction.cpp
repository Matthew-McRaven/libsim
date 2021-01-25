#include "isa/pep10/instruction.hpp"

#include <iostream>
#include <algorithm>
#include <map>

using namespace isa::pep10;
const std::map<instruction_mnemonic, instruction_definition<uint8_t>> init_isa() {
	return {
		{instruction_mnemonic::RET, {0x00, addressing_class::U_none,   {{false,false,false,false,}}, instruction_mnemonic::RET, true, ""}},
		{instruction_mnemonic::SRET, {0x01, addressing_class::U_none,   {{false,false,false,false,}}, instruction_mnemonic::SRET, true, ""}},
		{instruction_mnemonic::MOVSPA, {0x02, addressing_class::U_none,   {{false,false,false,false,}}, instruction_mnemonic::MOVSPA, true, ""}},
		{instruction_mnemonic::MOVASP, {0x03, addressing_class::U_none,   {{false,false,false,false,}}, instruction_mnemonic::MOVASP, true,""}},
		{instruction_mnemonic::MOVFLGA, {0x04, addressing_class::U_none,   {{false,false,false,false,}}, instruction_mnemonic::MOVFLGA, true,""}},
		{instruction_mnemonic::MOVAFLG, {0x05, addressing_class::U_none,   {{ true, true, true, true,}},  instruction_mnemonic::MOVAFLG, true,""}},
		{instruction_mnemonic::MOVTA, {0x06, addressing_class::U_none,   {{false,false,false,false,}}, instruction_mnemonic::MOVTA, true, ""}},
		{instruction_mnemonic::USCALL, {0x07, addressing_class::U_none,   {{false,false,false,false,}}, instruction_mnemonic::USCALL, true, ""}},
		{instruction_mnemonic::NOP, {0x08, addressing_class::U_none,   {{false,false,false,false,}}, instruction_mnemonic::NOP, true, ""}},
		{instruction_mnemonic::UNIMPL, {0x09, addressing_class::Invalid,  {{false,false,false,false,}}, instruction_mnemonic::UNIMPL, true, ""}},
		//GAP

		
		{instruction_mnemonic::NOTA, {0x10, addressing_class::R_none, {{ true, true,false,false}}, instruction_mnemonic::NOTA, true, ""}},
		{instruction_mnemonic::NOTX, {0x11, addressing_class::R_none, {{ true, true,false,false}}, instruction_mnemonic::NOTX, true, ""}},
		{instruction_mnemonic::NEGA, {0x12, addressing_class::R_none, {{ true, true, true,false}}, instruction_mnemonic::NEGA, true, ""}},
		{instruction_mnemonic::NEGX, {0x13, addressing_class::R_none, {{ true, true, true,false}}, instruction_mnemonic::NEGX, true, ""}},
		{instruction_mnemonic::ASLA, {0x14, addressing_class::R_none, {{ true, true, true, true}}, instruction_mnemonic::ASLA, true, ""}},
		{instruction_mnemonic::ASLX, {0x15, addressing_class::R_none, {{ true, true, true, true}}, instruction_mnemonic::ASLX, true, ""}},
		{instruction_mnemonic::ASRA, {0x16, addressing_class::R_none, {{ true, true,false, true}}, instruction_mnemonic::ASRA, true, ""}},
		{instruction_mnemonic::ASRX, {0x17, addressing_class::R_none, {{ true, true,false, true}}, instruction_mnemonic::ASRX, true, ""}},
		{instruction_mnemonic::ROLA, {0x18, addressing_class::R_none, {{false,false,false, true}}, instruction_mnemonic::ROLA, true, ""}},
		{instruction_mnemonic::ROLX, {0x19, addressing_class::R_none, {{false,false,false, true}}, instruction_mnemonic::ROLX, true, ""}},
		{instruction_mnemonic::RORA, {0x1A, addressing_class::R_none, {{false,false,false, true}}, instruction_mnemonic::RORA, true, ""}},
		{instruction_mnemonic::RORX, {0x1B, addressing_class::R_none, {{false,false,false, true}}, instruction_mnemonic::RORX, true, ""}},

		{instruction_mnemonic::BR, {0x1C, addressing_class::A_ix, {{false,false,false,false,}}, instruction_mnemonic::BR, false, ""}},
		{instruction_mnemonic::BRLE, {0x1E, addressing_class::A_ix, {{false,false,false,false,}}, instruction_mnemonic::BRLE, false, ""}},
		{instruction_mnemonic::BRLT, {0x20, addressing_class::A_ix, {{false,false,false,false,}}, instruction_mnemonic::BRLT, false, ""}},
		{instruction_mnemonic::BREQ, {0x22, addressing_class::A_ix, {{false,false,false,false,}}, instruction_mnemonic::BREQ, false, ""}},
		{instruction_mnemonic::BRNE, {0x24, addressing_class::A_ix, {{false,false,false,false,}}, instruction_mnemonic::BRNE, false, ""}},
		{instruction_mnemonic::BRGE, {0x26, addressing_class::A_ix, {{false,false,false,false,}}, instruction_mnemonic::BRGE, false, ""}},
		{instruction_mnemonic::BRGT, {0x28, addressing_class::A_ix, {{false,false,false,false,}}, instruction_mnemonic::BRGT, false, ""}},
		{instruction_mnemonic::BRV, {0x2A, addressing_class::A_ix, {{false,false,false,false,}}, instruction_mnemonic::BRV, false, ""}},
		{instruction_mnemonic::BRC, {0x2C, addressing_class::A_ix, {{false,false,false,false,}}, instruction_mnemonic::BRC, false, ""}},
		{instruction_mnemonic::CALL, {0x2E, addressing_class::A_ix, {{false,false,false,false,}}, instruction_mnemonic::CALL, false, ""}},

		{instruction_mnemonic::SCALL, {0x30, addressing_class::AAA_all, {{false,false,false,false,}}, instruction_mnemonic::SCALL, true, ""}},
		{instruction_mnemonic::LDWT, {0x38, addressing_class::AAA_i,   {{false,false,false,false,}}, instruction_mnemonic::LDWT, false, ""}},

		{instruction_mnemonic::LDWA, {0x40, addressing_class::RAAA_all, {{ true, true,false,false,}}, instruction_mnemonic::LDWA, false, ""}},
		{instruction_mnemonic::LDWX, {0x48, addressing_class::RAAA_all, {{ true, true,false,false,}}, instruction_mnemonic::LDWX, false, ""}},
		{instruction_mnemonic::LDBA, {0x50, addressing_class::RAAA_all, {{ true, true,false,false,}}, instruction_mnemonic::LDBA, false, ""}},
		{instruction_mnemonic::LDBX, {0x58, addressing_class::RAAA_all, {{ true, true,false,false,}}, instruction_mnemonic::LDBX, false, ""}},

		{instruction_mnemonic::STWA, {0x60, addressing_class::RAAA_noi, {{false,false,false,false,}}, instruction_mnemonic::STWA, false, ""}},
		{instruction_mnemonic::STWX, {0x68, addressing_class::RAAA_noi, {{false,false,false,false,}}, instruction_mnemonic::STWX, false, ""}},
		{instruction_mnemonic::STBA, {0x70, addressing_class::RAAA_noi, {{false,false,false,false,}}, instruction_mnemonic::STBA, false, ""}},
		{instruction_mnemonic::STBX, {0x78, addressing_class::RAAA_noi, {{false,false,false,false,}}, instruction_mnemonic::STBX, false, ""}},

		{instruction_mnemonic::CPWA, {0x80, addressing_class::RAAA_all, {{ true, true, true, true,}}, instruction_mnemonic::CPWA, false, ""}},
		{instruction_mnemonic::CPWX, {0x88, addressing_class::RAAA_all, {{ true, true, true, true,}}, instruction_mnemonic::CPWX, false, ""}},
		{instruction_mnemonic::CPBA, {0x90, addressing_class::RAAA_all, {{ true, true, true, true,}}, instruction_mnemonic::CPBA, false, ""}},
		{instruction_mnemonic::CPBX, {0x98, addressing_class::RAAA_all, {{ true, true, true, true,}}, instruction_mnemonic::CPBX, false, ""}},
		{instruction_mnemonic::ADDA, {0xa0, addressing_class::RAAA_all, {{ true, true, true, true,}}, instruction_mnemonic::ADDA, false, ""}},
		{instruction_mnemonic::ADDX, {0xa8, addressing_class::RAAA_all, {{ true, true, true, true,}}, instruction_mnemonic::ADDX, false, ""}},
		{instruction_mnemonic::SUBA, {0xb0, addressing_class::RAAA_all, {{ true, true, true, true,}}, instruction_mnemonic::SUBA, false, ""}},
		{instruction_mnemonic::SUBX, {0xb8, addressing_class::RAAA_all, {{ true, true, true, true,}}, instruction_mnemonic::SUBX, false, ""}},
		{instruction_mnemonic::ANDA, {0xc0, addressing_class::RAAA_all, {{ true, true,false,false,}}, instruction_mnemonic::ANDA, false, ""}},
		{instruction_mnemonic::ANDX, {0xc8, addressing_class::RAAA_all, {{ true, true,false,false,}}, instruction_mnemonic::ANDX, false, ""}},
		{instruction_mnemonic::ORA, {0xd0, addressing_class::RAAA_all, {{ true, true,false,false,}}, instruction_mnemonic::ORA, false, ""}},
		{instruction_mnemonic::ORX, {0xd8, addressing_class::RAAA_all, {{ true, true,false,false,}}, instruction_mnemonic::ORX, false, ""}},
		{instruction_mnemonic::XORA, {0xe0, addressing_class::RAAA_all, {{ true, true,false,false,}}, instruction_mnemonic::XORA, false, ""}},
		{instruction_mnemonic::XORX, {0xe8, addressing_class::RAAA_all, {{ true, true,false,false,}}, instruction_mnemonic::XORX, false, ""}},

		{instruction_mnemonic::ADDSP, {0xf0, addressing_class::AAA_all, {{ true, true, true, true}}, instruction_mnemonic::ADDSP, false, ""}},
		{instruction_mnemonic::SUBSP, {0xf8, addressing_class::AAA_all, {{ true, true, true, true}}, instruction_mnemonic::SUBSP, false, ""}}
	};
}
static const auto isa_map = init_isa();

std::array<isa::pep10::addr_map, 256> init_mmap() {
	using instr = instruction_definition<uint8_t>;
	std::array<isa::pep10::addr_map, 256> riproll = {};
	for(int it=0; it<256; it++) {
			using tp = const std::pair<const isa::pep10::instruction_mnemonic, isa::pep10::instruction_definition<uint8_t> >;
			auto ub = std::upper_bound(isa_map.cbegin(), isa_map.cend(), it, [](int v, const tp &ref){return v < ref.second.bit_pattern ;});

			// First element is >, so none are <=
			if(ub == isa_map.cbegin()) {
				throw std::invalid_argument("Invalid Instruction.");
			}
			auto lb = *(--ub);


			addressing_mode mode = addressing_mode::INVALID;

			switch (lb.second.iformat) {
			case addressing_class::R_none:
				[[fallthrough]]; 
			case addressing_class::U_none: 
				mode = addressing_mode::NONE;
				break;
			case addressing_class::A_ix:
				if (it-lb.second.bit_pattern == 0) mode = addressing_mode::I;
				else if (it-lb.second.bit_pattern == 1) mode = addressing_mode::X;
				else throw std::invalid_argument("Invalid A addressing mode.");
				break;
			case addressing_class::AAA_i:
				if (it-lb.second.bit_pattern == 0) mode = addressing_mode::I;
				else if (it-lb.second.bit_pattern <= 7) mode = addressing_mode::INVALID;
				else throw std::invalid_argument("Invalid AAA_i addressing mode.");
				break;
			case addressing_class::AAA_all:
				[[fallthrough]];
			case addressing_class::RAAA_all:
				if (it-lb.second.bit_pattern == 0) mode = addressing_mode::I;
				else if (it-lb.second.bit_pattern == 1) mode = addressing_mode::D;
				else if (it-lb.second.bit_pattern == 2) mode = addressing_mode::N;
				else if (it-lb.second.bit_pattern == 3) mode = addressing_mode::S;
				else if (it-lb.second.bit_pattern == 4) mode = addressing_mode::SF;
				else if (it-lb.second.bit_pattern == 5) mode = addressing_mode::X;
				else if (it-lb.second.bit_pattern == 6) mode = addressing_mode::SX;
				else if (it-lb.second.bit_pattern == 7) mode = addressing_mode::SFX;
				else throw std::invalid_argument("Invalid RAAA addressing mode.");
				break;
			case addressing_class::RAAA_noi:
				if (it-lb.second.bit_pattern == 0) mode = addressing_mode::INVALID;
				else if (it-lb.second.bit_pattern == 1) mode = addressing_mode::D;
				else if (it-lb.second.bit_pattern == 2) mode = addressing_mode::N;
				else if (it-lb.second.bit_pattern == 3) mode = addressing_mode::S;
				else if (it-lb.second.bit_pattern == 4) mode = addressing_mode::SF;
				else if (it-lb.second.bit_pattern == 5) mode = addressing_mode::X;
				else if (it-lb.second.bit_pattern == 6) mode = addressing_mode::SX;
				else if (it-lb.second.bit_pattern == 7) mode = addressing_mode::SFX;
				else throw std::invalid_argument("Invalid AAA addressing mode.");
				break;
			}
			riproll[it].inst = lb.second;
			riproll[it].addr = mode;
			
	}
	return riproll;
}

static const isa_definition definition = isa::pep10::isa_definition();

isa_definition::isa_definition():
	isa(isa_map), riproll(init_mmap())
{

}

const isa_definition&  isa_definition::get_definition()
{
	return definition;
}

std::string isa::pep10::as_string(instruction_mnemonic mnemon) {
	if (mnemon == instruction_mnemonic::UNIMPL || mnemon == instruction_mnemonic::MAX) {
		throw std::invalid_argument("Not a real mnemonic");
	}
	return std::string(magic_enum::enum_name(mnemon));
}

bool isa::pep10::is_opcode_unary(instruction_mnemonic mnemon)
{
	auto addr_class = definition.isa.at(mnemon).iformat;
	switch (addr_class)
	{
	case addressing_class::A_ix:
	case addressing_class::AAA_all:
	case addressing_class::AAA_i:
	case addressing_class::RAAA_all:
	case addressing_class::RAAA_noi:
		return true;
	case addressing_class::U_none:
	case addressing_class::R_none:
		return false;
	}
	throw std::invalid_argument("Invalid opcode.");
}

bool isa::pep10::is_opcode_unary(uint8_t opcode)
{
	auto [inst, addr] = definition.riproll[opcode];
	return is_opcode_unary(inst.mnemonic);
}
bool isa::pep10::is_store(instruction_mnemonic mnemon)
{
	if(mnemon == instruction_mnemonic::STBA ||
		mnemon == instruction_mnemonic::STWA) return true;
	else return false;
}
bool isa::pep10::is_store(uint8_t opcode)
{
	auto [inst, addr] = definition.riproll[opcode];
	return is_store(inst.mnemonic);
}
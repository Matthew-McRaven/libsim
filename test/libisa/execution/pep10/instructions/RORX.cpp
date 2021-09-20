#include <bit>
#include <cstdint>

#include <catch.hpp>
#include <fmt/core.h>

#include "components/storage/base.hpp"
#include "components/storage/block.hpp"
#include "isa/pep10/local_machine.hpp"
#include "isa/pep10/local_processor.hpp"
#include "isa/pep10/pep10.hpp"


TEST_CASE("Instruction: RORX", "[isa::pep10]")
{
	auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
	auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
	// Object code for instruction under test.
	std::vector<uint8_t> program = {0x1B};
	// RTL: C ← X[15] , X[1 : 15] ← X[0 : 14] , r[0] ← X
	SECTION("RORX")
	{
		// Loop over non-target status bit combinations to ensure that the instruction does not modify non-target bits.
		for(uint8_t nzvc = 0; static_cast<uint8_t>(nzvc)+1<0b1'0000; nzvc++)
		{
			for(uint16_t X=0; static_cast<uint32_t>(X)+1<0x1'0000;X++)
			{
				machine->clear_all(0, 0, false);
				machine->write_register(isa::pep10::Register::X, X);
				machine->write_packed_csr(nzvc);
				isa::pep10::load_bytes(machine, program, 0).value();
				auto start_n = nzvc & 0x8 ? 1 : 0;
				auto start_z = nzvc & 0x4 ? 1 : 0;
				auto start_v = nzvc & 0x2 ? 1 : 0;
				auto start_c = nzvc & 0x1 ? 1 : 0;


				auto ret = machine->step();
				REQUIRE(ret.has_value());
				CHECK(ret.value() == step::Result::kNominal);

				// Check that other registers were not mutated.
				CHECK(machine->read_register(isa::pep10::Register::SP) == 0);
				CHECK(machine->read_register(isa::pep10::Register::A) == 0);
				CHECK(machine->read_register(isa::pep10::Register::OS) == 0);
				CHECK(machine->read_register(isa::pep10::Register::TR) == 0);
				// PC was incremented by one byte
				CHECK(machine->read_register(isa::pep10::Register::PC) == 0x01);
				// IS has the correct instruction mnemonic
				CHECK(machine->read_register(isa::pep10::Register::IS) == 0x1B);
				auto new_X = machine->read_register(isa::pep10::Register::X);
				// X is the starting A, shifted right by 1 bit. The carry is shifted into X[0].
				CHECK(new_X == (static_cast<uint16_t>(X>>1)|(start_c<<15)));
				CHECK(machine->read_csr(isa::pep10::CSR::N) == start_n);
				CHECK(machine->read_csr(isa::pep10::CSR::Z) == start_z);
				CHECK(machine->read_csr(isa::pep10::CSR::V) == start_v);
				// Carry out if high order bit was non-zero
				CHECK(machine->read_csr(isa::pep10::CSR::C) == ((X & 0x1) ? 1 : 0));
			}
		}
	}


}

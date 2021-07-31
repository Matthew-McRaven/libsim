#include <bit>
#include <cstdint>

#include <catch.hpp>
#include <fmt/core.h>

#include "components/storage/base.hpp"
#include "components/storage/block.hpp"
#include "isa/pep10/local_machine.hpp"
#include "isa/pep10/local_processor.hpp"
#include "isa/pep10/pep10.hpp"


TEST_CASE("Instruction: ASLA", "[isa::pep10]")
{
	auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
	auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
	std::vector<uint8_t> program = {0x14};
	SECTION("ASLA")
	{
		for(uint16_t A=0; static_cast<uint32_t>(A)+1<0x1'0000;A++)
		{
			machine->clear_all(0, 0, false);
			// RTL: C ← A[0] , A[0 : 14] ← A[1 : 15] , A[15] ← 0 ; N ← A < 0 , Z ← A = 0 , V ← {overflow}
			// Object code for `NOTA`
			machine->write_register(isa::pep10::Register::A, A);
			isa::pep10::load_bytes(machine, program, 0).value();



			auto ret = machine->step();
			REQUIRE(ret.has_value());
			CHECK(ret.value());

			// Check that other registers were not mutated.
			CHECK(machine->read_register(isa::pep10::Register::SP) == 0);
			CHECK(machine->read_register(isa::pep10::Register::X) == 0);
			CHECK(machine->read_register(isa::pep10::Register::OS) == 0);
			CHECK(machine->read_register(isa::pep10::Register::TR) == 0);
			// PC was incremented by one byte
			CHECK(machine->read_register(isa::pep10::Register::PC) == 0x01);
			// IS has the correct instruction mnemonic
			CHECK(machine->read_register(isa::pep10::Register::IS) == 0x14);
			// A is the bitwise negation of starting A
			auto new_A = machine->read_register(isa::pep10::Register::A);
			// Count the number of 1's in A[0:1].
			// If it is 0 or 2, then they agree in sign -> no signed overflow.
			// Otherwise, they disagree in sign -> signed overflow.
			auto top_2_bits = std::popcount(static_cast<uint16_t>(A>>14));
			CHECK(new_A == static_cast<uint16_t>(A<<1));
			CHECK(machine->read_csr(isa::pep10::CSR::N) == ((new_A & 0x8000) ? 1 : 0));
			CHECK(machine->read_csr(isa::pep10::CSR::Z) == ((new_A == 0) ? 1 : 0));
			// Signed overflow if the original top two bits contain opposite values.
			CHECK(machine->read_csr(isa::pep10::CSR::V) == ((top_2_bits % 2) ? 1 : 0));
			// Carry out if high order bit was non-zero
			CHECK(machine->read_csr(isa::pep10::CSR::C) == ((A & 0x8000) ? 1 : 0));
		}
	}


}

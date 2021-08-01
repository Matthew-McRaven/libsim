#include <bit>
#include <cstdint>
#include <iostream>

#include <catch.hpp>
#include <fmt/core.h>

#include "components/storage/base.hpp"
#include "components/storage/block.hpp"
#include "isa/pep10/local_machine.hpp"
#include "isa/pep10/local_processor.hpp"
#include "isa/pep10/pep10.hpp"


TEST_CASE("Instruction: SUBSP,i", "[isa::pep10]")
{
	auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
	auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
	static const auto target_reg = isa::pep10::Register::SP;
	// RTL: SP ← SP - Oprnd
	SECTION("SUBSP, i")
	{
		auto target_reg_values = std::vector<uint16_t>{0x0000, 0x0001, 0x7fff, 0x8000, 0x8FFF, 0xFFFF};
		// Loop over a subset of possible values for the target register.
		for(auto target_reg_val : target_reg_values)
		{
			for(uint8_t start_stat = 0; start_stat < 0b1'0000; start_stat++) {
				for(uint16_t opspec=0; static_cast<uint32_t>(opspec)+1<0x1'0000;opspec++)
				{
					// Object code for instruction under test.
					std::vector<uint8_t> program = {0xF8, static_cast<uint8_t>((opspec>>8)&0xff), 
						static_cast<uint8_t>(opspec&0xff)};
					machine->clear_all(0, 0, false);
					machine->write_register(target_reg, target_reg_val);
					machine->write_packed_csr(start_stat);
					isa::pep10::load_bytes(machine, program, 0).value();


					auto ret = machine->step();
					REQUIRE(ret.has_value());
					CHECK(ret.value());

					// Check that other registers were not mutated.
					CHECK(machine->read_register(isa::pep10::Register::X) == 0);
					CHECK(machine->read_register(isa::pep10::Register::A) == 0);
					CHECK(machine->read_register(isa::pep10::Register::TR) == 0);
					CHECK(machine->read_packed_csr() == start_stat);
					// PC incremented by 3.
					CHECK(machine->read_register(isa::pep10::Register::PC) == 0x03);
					// IS has the correct instruction mnemonic
					CHECK(machine->read_register(isa::pep10::Register::IS) == 0xF8);
					// OS loaded the Mem[0x0001-0x0002].
					CHECK(machine->read_register(isa::pep10::Register::OS) == opspec);
					// Check that target register had arithmetic performed.
					auto new_target = machine->read_register(target_reg);
					CHECK(new_target == static_cast<uint16_t>(target_reg_val + (~opspec + 1)));

					
				}
			}
		}
	}


}

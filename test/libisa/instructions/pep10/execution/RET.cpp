#include <cstdint>

#include <catch.hpp>

#include "components/storage/base.hpp"
#include "components/storage/block.hpp"
#include "isa/pep10/local_machine.hpp"
#include "isa/pep10/local_processor.hpp"
#include "isa/pep10/pep10.hpp"


TEST_CASE("Instruction: RET", "[isa::pep10]")
{
	// Loop over all status bit combinations to ensure that RET does not modify status bits.
	// Rely on the packed_csr features of the machine to handle translating uint8_t to status bit assignments.
	for(uint8_t start_stat = 0; start_stat <= 0b1111; start_stat++)
	{
		// RTL: PC ← Mem[SP] ; SP ← SP + 2
		DYNAMIC_SECTION(fmt::format("RET: NZVC={:b}", start_stat))
		{
			auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
			auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
			
			machine->write_register(isa::pep10::Register::SP, 0x0004);
			// Set Mem[SP] to have a non-zero value.
			machine->write_memory(0x00004, 0xDE);
			machine->write_memory(0x00005, 0xAD);
			// Set the starting status bits so that we can check that they are not mutated by this instruction.
			machine->write_packed_csr(start_stat);

			auto ret = machine->step();
			REQUIRE(ret.has_value());
			CHECK(ret.value());

			// Check that other registers were not mutated.
			CHECK(machine->read_register(isa::pep10::Register::X) == 0);
			CHECK(machine->read_register(isa::pep10::Register::OS) == 0);
			CHECK(machine->read_register(isa::pep10::Register::TR) == 0);
			// Status bits did not change.
			CHECK(machine->read_packed_csr() == start_stat);
			// IS has the correct instruction mnemonic
			CHECK(machine->read_register(isa::pep10::Register::IS) == 0x00);
			// Pc has value at old SP loaded.
			CHECK(machine->read_register(isa::pep10::Register::PC) == 0xDEAD);
			// Stack pointer is incremented by one word.
			CHECK(machine->read_register(isa::pep10::Register::SP) == 0x0006);
			// Status bits did not change.
			CHECK(machine->read_packed_csr() == start_stat);
		}
	}


}

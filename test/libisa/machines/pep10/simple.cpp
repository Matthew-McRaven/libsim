#include <cstdint>

#include <catch.hpp>

#include "components/storage/base.hpp"
#include "components/storage/block.hpp"
#include "isa/pep10/local_machine.hpp"
#include "isa/pep10/local_processor.hpp"
#include "isa/pep10/pep10.hpp"

// Don't do any program simulation here.
TEST_CASE("Simple static tests of Pep/10 computer", "[isa::pep10]")
{
	SECTION("Test R/W registers")
	{
		auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0x1'0000);
		auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
		machine->write_register(isa::pep10::Register::A, 0xDEAD);
		machine->write_register(isa::pep10::Register::TR, 0xBEEF);
		CHECK(machine->read_register(isa::pep10::Register::A) == 0xDEAD);
		CHECK(machine->read_register(isa::pep10::Register::TR) == 0xBEEF);
	}
}

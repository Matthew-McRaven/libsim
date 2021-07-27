#include <cstdint>

#include <catch.hpp>

#include "components/storage/base.hpp"
#include "components/storage/block.hpp"
#include "isa/pep10/local_machine.hpp"
#include "isa/pep10/local_processor.hpp"
#include "isa/pep10/pep10.hpp"

// Don't do any program simulation here.
TEST_CASE("Simple execution trace tests of Pep/10 computer", "[isa::pep10]")
{
	SECTION("Test step() via RET")
	{
		auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0x1'0000);
		auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
		machine->write_register(isa::pep10::Register::SP, 0x0004);
		auto ret = machine->step();
		REQUIRE(ret.has_value());
		CHECK(ret.value());
		CHECK(machine->read_register(isa::pep10::Register::SP) == 0x0002);
	}

}

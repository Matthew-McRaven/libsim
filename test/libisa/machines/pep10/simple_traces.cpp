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
		auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
		auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
		machine->write_register(isa::pep10::Register::SP, 0x0004);
		auto ret = machine->step();
		REQUIRE(ret.has_value());
		CHECK(ret.value());
		CHECK(machine->read_register(isa::pep10::Register::SP) == 0x0006);
	}
	// Tests ability to bring data into A register.
	// Tests ability to execute nonunary instructions.
	// Tests load_bytes.
	// Tests correct endianess of operand decoding.
	SECTION("Test accumulator immediate load.")
	{
		auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
		auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
		// Object code for `LDWA 0x1234,i`
		auto x = std::vector<uint8_t>{0x40, 0x12, 0x34};
		isa::pep10::load_bytes(machine, x, 0).value();
		auto ret = machine->step();
		REQUIRE(ret.has_value());
		CHECK(ret.value());
		CHECK(machine->read_register(isa::pep10::Register::A) == 0x1234);
		CHECK(machine->read_register(isa::pep10::Register::PC) == 0x003);
	}

}

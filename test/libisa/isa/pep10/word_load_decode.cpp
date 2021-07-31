#include <cstdint>

#include <catch.hpp>

#include "components/storage/base.hpp"
#include "components/storage/block.hpp"
#include "isa/pep10/local_machine.hpp"
#include "isa/pep10/local_processor.hpp"
#include "isa/pep10/pep10.hpp"

// Don't do any program simulation here.
TEST_CASE("Test operand decoding for loading words", "[isa::pep10]")
{
	// Test addressing mode i
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
	}

	// Test addressing mode d.
	SECTION("Test accumulator load direct.")
	{
		auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
		auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
		// Object code for `LDWA 0x0000,d`
		auto x = std::vector<uint8_t>{0x41, 0x00, 0x03, 0xDE, 0xAD};
		isa::pep10::load_bytes(machine, x, 0).value();
		auto ret = machine->step();
		REQUIRE(ret.has_value());
		CHECK(ret.value());
		CHECK(machine->read_register(isa::pep10::Register::A) == 0xDEAD);
	}

	// Test addressing mode n.
	SECTION("Test accumulator load indirect.")
	{
		auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
		auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
		// Object code for `LDWA 0x0003,n\n.WORD 0x0000`
		// This should load `0x0003` into the A register
		auto x = std::vector<uint8_t>{0x42, 0x00, 0x03, 0x00, 0x05, 0xDE, 0xAD};
		isa::pep10::load_bytes(machine, x, 0).value();
		auto ret = machine->step();
		REQUIRE(ret.has_value());
		CHECK(ret.value());
		CHECK(machine->read_register(isa::pep10::Register::A) == 0xDEAD);
	}

	// Test addressing mode s.
	SECTION("Test accumulator load stack-relative.")
	{
		auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
		auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
		// Object code for `LDWA 0x0000,n\n.WORD 0xDEAD`
		// This should load `0xDEAD` into the A register
		auto x = std::vector<uint8_t>{0x43, 0x00, 0x01, 0xDE, 0xAD};
		isa::pep10::load_bytes(machine, x, 0).value();
		machine->write_register(isa::pep10::Register::SP, 0x0002);
		auto ret = machine->step();
		REQUIRE(ret.has_value());
		CHECK(ret.value());
		CHECK(machine->read_register(isa::pep10::Register::A) == 0xDEAD);
	}

	// Test addressing mode sf.
	SECTION("Test accumulator load stack-relative deferred.")
	{
		auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
		auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
		// Object code for `LDWA 0x0000,n\n.WORD 0xDEAD`
		// This should load `0xDEAD` into the A register
		auto x = std::vector<uint8_t>{0x44, 0x00, 0x02, 0x00, 0x05, 0xDE, 0xAD};
		isa::pep10::load_bytes(machine, x, 0).value();
		machine->write_register(isa::pep10::Register::SP, 0x0001);
		auto ret = machine->step();
		REQUIRE(ret.has_value());
		CHECK(ret.value());
		CHECK(machine->read_register(isa::pep10::Register::A) == 0xDEAD);
	}

	// Test addressing mode x.
	SECTION("Test accumulator load indexed.")
	{
		auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
		auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
		// Object code for `LDWA 0x0000,n\n.WORD 0xDEAD`
		// This should load `0xDEAD` into the A register
		auto x = std::vector<uint8_t>{0x45, 0x00, 0x01, 0xDE, 0xAD};
		isa::pep10::load_bytes(machine, x, 0).value();
		machine->write_register(isa::pep10::Register::X, 0x0002);
		auto ret = machine->step();
		REQUIRE(ret.has_value());
		CHECK(ret.value());
		CHECK(machine->read_register(isa::pep10::Register::A) == 0xDEAD);
	}

	// Test addressing mode sx.
	SECTION("Test accumulator load stack-indexed.")
	{
		auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
		auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
		// Object code for `LDWA 0x0000,n\n.WORD 0xDEAD`
		// This should load `0xDEAD` into the A register
		auto x = std::vector<uint8_t>{0x46, 0x00, 0x01, 0xDE, 0xAD};
		isa::pep10::load_bytes(machine, x, 0).value();
		machine->write_register(isa::pep10::Register::X, 0x0001);
		machine->write_register(isa::pep10::Register::SP, 0x0001);
		auto ret = machine->step();
		REQUIRE(ret.has_value());
		CHECK(ret.value());
		CHECK(machine->read_register(isa::pep10::Register::A) == 0xDEAD);
	}

	// Test addressing mode sfx.
	SECTION("Test accumulator load stack-deffered indexed.")
	{
		auto storage = std::make_shared<components::storage::Block<uint16_t, true, uint8_t>>(0xFFFF);
		auto machine = std::make_shared<isa::pep10::LocalMachine<true>>(storage);
		// Object code for `LDWA 0x0000,n\n.WORD 0xDEAD`
		// This should load `0xDEAD` into the A register
		auto x = std::vector<uint8_t>{0x47, 0x00, 0x01, 0xDE, 0xAD, 0x00, 0x02};
		isa::pep10::load_bytes(machine, x, 0).value();
		machine->write_register(isa::pep10::Register::X, 0x0001);
		machine->write_register(isa::pep10::Register::SP, 0x0004);
		auto ret = machine->step();
		REQUIRE(ret.has_value());
		CHECK(ret.value());
		CHECK(machine->read_register(isa::pep10::Register::A) == 0xDEAD);
	}

}
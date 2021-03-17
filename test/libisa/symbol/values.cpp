#include "catch.hpp"

#include "symbol/value.hpp"


TEST_CASE( "Validate functionality of symbol table." ) {
	
	// TODO: Add symbol table unit tests
	SECTION("Empty Value") {
		auto value = symbol::value_empty<uint16_t>();
		CHECK_NOFAIL(value.value());
		CHECK(value.value() == 0);
	}

	// Check that the values on a numeric value can be mutated.
	SECTION("Numeric Value") {
		auto start=30, end=20;
		auto value = symbol::value_const<uint16_t>(start);
		CHECK_NOFAIL(value.value());
		CHECK(value.value() == start);
		CHECK_NOTHROW(value.set_value(end));
		CHECK(value.value() == end);
	}

	// Check that the values on a location value can be mutated.
	SECTION("Location Value") {
		auto base=7;
		auto start_offset=11, end_offset=13;
		auto value = symbol::value_location<uint16_t>(base, start_offset);
		CHECK(value.value() == base + start_offset);
		CHECK_NOTHROW(value.set_offset(end_offset));
		CHECK(value.value() == base + end_offset);
		CHECK(value.relocatable());
	}
	// Can't test external symbol value here, as it will require a symbol table.
}
#include "catch.hpp"

#include "symbol/value.hpp"


TEST_CASE( "Validate functionality of symbol table." ) {
	
	// TODO: Add symbol table unit tests
	SECTION("Empty Value") {
		auto value = symbol::SymbolValueEmpty<uint16_t>();
		CHECK_NOFAIL(value.getValue());
		CHECK(value.getValue() == 0);
	}

	// Check that the values on a numeric value can be mutated.
	SECTION("Numeric Value") {
		auto start=30, end=20;
		auto value = symbol::SymbolValueNumeric<uint16_t>(start);
		CHECK_NOFAIL(value.getValue());
		CHECK(value.getValue() == start);
		CHECK_NOTHROW(value.setValue(end));
		CHECK(value.getValue() == end);
	}

	// Check that the values on a location value can be mutated.
	SECTION("Location Value") {
		auto start_base=7, end_base=3;
		auto start_offset=11, end_offset=13;
		auto value = symbol::SymbolValueLocation<uint16_t>(start_base, start_offset);
		CHECK(value.getValue() == start_base + start_offset);
		CHECK_NOTHROW(value.setOffset(end_offset));
		CHECK(value.getValue() == start_base + end_offset);
		CHECK_NOTHROW(value.setBase(end_base));
		CHECK(value.getValue() == end_base + end_offset);
		CHECK(value.canRelocate());
	}
	// Can't test external symbol value here, as it will require a symbol table.
}
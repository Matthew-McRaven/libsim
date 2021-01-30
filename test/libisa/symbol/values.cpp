#include "catch.hpp"

#include "symbol/value.hpp"


TEST_CASE( "Debug symbol values" ) {
	
	// TODO: Add symbol table unit tests
	SECTION("Add a single macro") {
		auto value = symbol::SymbolValueEmpty<uint16_t>();
		CHECK_NOFAIL(value.getValue());
		CHECK(value.getValue() == 0);

	}
	SECTION("overwrite existing macro") {
	}
}
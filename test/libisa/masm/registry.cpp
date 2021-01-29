#include "catch.hpp"

#include "isa/pep10/instruction.hpp"
#include "masm/registry.hpp"

TEST_CASE( "Macro definitions" ) {
    auto registry = masm::macro_registry();
	SECTION("Valid definitions") {
		CHECK(registry.register_macro("HELLOWORLD", "@HELLOWORLD 2\n", masm::MacroType::CoreMacro));
		CHECK(registry.contains("HELLOWORLD"));
	}

}

#include "catch.hpp"

#include "isa/pep10/instruction.hpp"
#include "masm/registry.hpp"

TEST_CASE( "Valid Macro Definitions" ) {
    auto registry = masm::macro_registry();
	SECTION("Add a single macro") {
		CHECK(registry.register_macro("HELLOWORLD", "@HELLOWORLD 2\n", masm::MacroType::CoreMacro));
		CHECK(registry.contains("HELLOWORLD"));
		auto macro = registry.macro("HELLOWORLD");
		CHECK(macro->arg_count == 2);
		CHECK(registry.contains("helloworld"));
	}
}

TEST_CASE( "Invalid Macro Definitions" ) {
    auto registry = masm::macro_registry();

	SECTION("Too many args") {
		CHECK_FALSE(registry.register_macro("HELLOWORLD", "@HELLOWORLD 2 3\n", masm::MacroType::CoreMacro));
		CHECK_FALSE(registry.contains("HELLOWORLD"));
		CHECK_FALSE(registry.contains("helloworld"));
		registry.clear_macros(masm::MacroType::CoreMacro);
	}

	SECTION("Exclude leading spaces") {
		CHECK_FALSE(registry.register_macro("HELLOWORLD", "  @HELLOWORLD 2\n", masm::MacroType::CoreMacro));
		CHECK_FALSE(registry.contains("HELLOWORLD"));
		CHECK_FALSE(registry.contains("helloworld"));
		registry.clear_macros(masm::MacroType::CoreMacro);
	}

	SECTION("Exclude re-registration") {
		CHECK(registry.register_macro("HELLOWORLD", "@HELLOWORLD 2\n", masm::MacroType::CoreMacro));
		CHECK_FALSE(registry.register_macro("HELLOWORLD", "@HELLOWORLD 4\n", masm::MacroType::CoreMacro));
		CHECK(registry.contains("HELLOWORLD"));
		registry.clear_macros(masm::MacroType::CoreMacro);
	}

	SECTION("Name mismatch") {
		CHECK_FALSE(registry.register_macro("goodbyeworld", "@HELLOWORLD 2\n", masm::MacroType::CoreMacro));
		CHECK_FALSE(registry.contains("HELLOWORLD"));
		CHECK_FALSE(registry.contains("goodbyeworld"));
		registry.clear_macros(masm::MacroType::CoreMacro);
	}

	SECTION("Missing arg count") {
		CHECK_FALSE(registry.register_macro("HELLOWORLD", "@HELLOWORLD\n", masm::MacroType::CoreMacro));
		CHECK_FALSE(registry.contains("HELLOWORLD"));
		CHECK_FALSE(registry.contains("helloworld"));
		registry.clear_macros(masm::MacroType::CoreMacro);
	}

	SECTION("Missing @") {
		CHECK_FALSE(registry.register_macro("HELLOWORLD", "HELLOWORLD 2\n", masm::MacroType::CoreMacro));
		CHECK_FALSE(registry.contains("HELLOWORLD"));
		CHECK_FALSE(registry.contains("helloworld"));
		registry.clear_macros(masm::MacroType::CoreMacro);
	}

}
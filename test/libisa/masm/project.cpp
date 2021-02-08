#include "catch.hpp"

#include "masm/project/section.hpp"
#include "masm/registry.hpp"

TEST_CASE( "Section linking" ) {
    
	SECTION("Create top level section & link to macro") {
		auto tl = std::make_shared<masm::elf::top_level_section<uint16_t>>();
		// Must alias tl so that it can be parsed as a code_section.
		// Ref count on both ptrs will be 2, so this should be safe.
		auto tl_as_code = std::shared_ptr<masm::elf::code_section<uint16_t>>(tl);

		masm::macro_registry registry;
		REQUIRE(registry.register_macro("hello", "@hello 0\nLDWA 20,x\n.END\n", masm::MacroType::CoreMacro));
		auto result = masm::elf::add_macro_subsection(registry, tl, 
			tl_as_code, 0, "hello", {}
		);

		// But jettison the temporary copy first, just to be safe.
		tl_as_code.reset();

		CHECK(!result.message);
		CHECK(result.section_ptr);
		CHECK(result.section_ptr.value()->direct_parent.lock() == tl);
		CHECK(result.section_ptr.value()->containing_section.lock() == tl);
	}

	SECTION("Nested linking") {
		auto tl = std::make_shared<masm::elf::top_level_section<uint16_t>>();
		// Must alias tl so that it can be parsed as a code_section.
		// Ref count on both ptrs will be 2, so this should be safe.
		auto tl_as_code = std::shared_ptr<masm::elf::code_section<uint16_t>>(tl);

		masm::macro_registry registry;
		REQUIRE(registry.register_macro("hello0", "@hello0 0\nLDWA 20,x\n.END\n", masm::MacroType::CoreMacro));
		REQUIRE(registry.register_macro("hello1", "@hello1 0\n@hell0\n.END\n", masm::MacroType::CoreMacro));

		auto mid_level = masm::elf::add_macro_subsection(registry, tl, tl_as_code, 0, "hello1", {});
		
		// But jettison the temporary copy first, just to be safe.
		tl_as_code.reset();

		CHECK(!mid_level.message);
		CHECK(mid_level.section_ptr);
		CHECK(mid_level.section_ptr.value()->direct_parent.lock() == tl);
		CHECK(mid_level.section_ptr.value()->containing_section.lock() == tl);

		// Must alias tl so that it can be parsed as a code_section.
		// Ref count on both ptrs will be 2, so this should be safe.
		auto ml_as_code = std::shared_ptr<masm::elf::code_section<uint16_t>>(mid_level.section_ptr.value());
		auto bottom_level = masm::elf::add_macro_subsection(registry, tl, 
			ml_as_code, 0, "hello0", {}
		);
		
		CHECK(!bottom_level.message);
		CHECK(bottom_level.section_ptr);
		CHECK(bottom_level.section_ptr.value()->direct_parent.lock() == mid_level.section_ptr.value());
		CHECK(bottom_level.section_ptr.value()->containing_section.lock() == tl);
	}

}
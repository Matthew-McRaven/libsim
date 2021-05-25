#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "masm/ir/directives.hpp"
#include "masm/ir/macro.hpp"

TEST_CASE( "Parse entire OS", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("0-arity macro.") {	
		auto project = masm::project::init_project<uint16_t>();
		CHECK(project->macro_registry->register_macro("HELLO2", "@HELLO2 2\nASRA\nASRA\n", masm::MacroType::CoreMacro));
		CHECK(project->macro_registry->register_macro("HELLO0", "@HELLO0 0\nASRA\nASRA\n", masm::MacroType::CoreMacro));
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "@HELLO0\n.END\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);

		REQUIRE(res.first);
		auto x = project->images[0]->section;
		REQUIRE(project->images[0]->section->body_ir->ir_lines.size() == 2);
		auto maybe_macro = project->images[0]->section->body_ir->ir_lines[0];
		auto as_macro = std::dynamic_pointer_cast<masm::ir::macro_invocation<uint16_t>>(maybe_macro);
		REQUIRE(as_macro);
		CHECK(as_macro->macro->body_ir->ir_lines.size() == 2);
		CHECK(!as_macro->comment);
	}

}
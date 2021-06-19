#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "masm/ir/directives.hpp"
#include "masm/ir/macro.hpp"

TEST_CASE( "Parse macro instructions", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("0-arity macro.") {	
		auto project = masm::project::init_project<uint16_t>();
		CHECK(project->macro_registry->register_macro("HELLO2", "@HELLO2 2\nASRA\nASRA\n", masm::MacroType::CoreMacro));
		CHECK(project->macro_registry->register_macro("HELLO0", "@HELLO0 0\nASRA\nASRA\n", masm::MacroType::CoreMacro));
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "@HELLO0\n.END\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);

		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 2);
		auto maybe_macro = project->image->os->body_ir->ir_lines[0];
		auto as_macro = std::dynamic_pointer_cast<masm::ir::macro_invocation<uint16_t>>(maybe_macro);
		REQUIRE(as_macro);
		CHECK(as_macro->macro->body_ir->ir_lines.size() == 2);
		CHECK(!as_macro->comment);
	}

	SECTION("2-arity macro.") {	
		auto project = masm::project::init_project<uint16_t>();
		CHECK(project->macro_registry->register_macro("HELLO2", "@HELLO2 2\nASRA\nASRA\n", masm::MacroType::CoreMacro));
		CHECK(project->macro_registry->register_macro("HELLO0", "@HELLO0 0\nASRA\nASRA\n", masm::MacroType::CoreMacro));
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "@HELLO2 a,b\n.END\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);

		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 2);
		auto maybe_macro = project->image->os->body_ir->ir_lines[0];
		auto as_macro = std::dynamic_pointer_cast<masm::ir::macro_invocation<uint16_t>>(maybe_macro);
		REQUIRE(as_macro);
		CHECK(as_macro->macro->body_ir->ir_lines.size() == 2);
		CHECK(!as_macro->comment);
	}

	SECTION("2-arity macro with a 0-arity macro.") {	
		auto project = masm::project::init_project<uint16_t>();
		CHECK(project->macro_registry->register_macro("HELLO2", "@HELLO2 2\n@HELLO0\n@HELLO0\n", masm::MacroType::CoreMacro));
		CHECK(project->macro_registry->register_macro("HELLO0", "@HELLO0 0\nASRA\nASRA\n", masm::MacroType::CoreMacro));
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "@HELLO2 a,b\n.END\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);

		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 2);
		auto maybe_macro = project->image->os->body_ir->ir_lines[0];
		auto as_macro = std::dynamic_pointer_cast<masm::ir::macro_invocation<uint16_t>>(maybe_macro);
		REQUIRE(as_macro);
		CHECK(as_macro->macro->body_ir->ir_lines.size() == 2);
		CHECK(!as_macro->comment);
	}

	SECTION("2-arity macro with a 0-arity macro, plus comment.") {	
		auto project = masm::project::init_project<uint16_t>();
		CHECK(project->macro_registry->register_macro("HELLO2", "@HELLO2 2\n@HELLO0\n@HELLO0\n", masm::MacroType::CoreMacro));
		CHECK(project->macro_registry->register_macro("HELLO0", "@HELLO0 0\nASRA\nASRA\n", masm::MacroType::CoreMacro));
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "@HELLO2 a,b;Hi guys\n.END\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);

		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 2);
		auto maybe_macro = project->image->os->body_ir->ir_lines[0];
		auto as_macro = std::dynamic_pointer_cast<masm::ir::macro_invocation<uint16_t>>(maybe_macro);
		REQUIRE(as_macro);
		CHECK(as_macro->macro->body_ir->ir_lines.size() == 2);
		CHECK(as_macro->comment);
	}

	SECTION("2-arity macro with a 0-arity macro, plus comment & symbol.") {	
		auto project = masm::project::init_project<uint16_t>();
		CHECK(project->macro_registry->register_macro("HELLO2", "@HELLO2 2\n@HELLO0\n@HELLO0\n", masm::MacroType::CoreMacro));
		CHECK(project->macro_registry->register_macro("HELLO0", "@HELLO0 0\nASRA\nASRA\n", masm::MacroType::CoreMacro));
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "lingus: @HELLO2 a,b;Hi guys\n.END\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);

		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 2);
		auto maybe_macro = project->image->os->body_ir->ir_lines[0];
		auto as_macro = std::dynamic_pointer_cast<masm::ir::macro_invocation<uint16_t>>(maybe_macro);
		REQUIRE(as_macro);
		CHECK(as_macro->symbol_entry);
		CHECK(as_macro->macro->body_ir->ir_lines.size() == 2);
		CHECK(as_macro->comment);
	}

}
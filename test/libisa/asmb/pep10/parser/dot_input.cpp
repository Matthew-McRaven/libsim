#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "asmb/pep10/directives.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot input", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("Valid .INPUT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:asra\n.INPUT s\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 2);
		auto maybe_input = project->image->os->body_ir->ir_lines[1];
		auto as_input = std::dynamic_pointer_cast<masm::ir::dot_input<uint16_t> >(maybe_input);
		REQUIRE(as_input);

		// Check for externalized symbol definition.
		REQUIRE(symbol::exists<uint16_t>({project->image->symbol_table}, "s"));
		// TODO: Check that "s" is marked as global
		// TODO: Check that there is only one external symbol.
		// TODO: Check that the list of external symbols starts with a symbol named "s".
		//auto externs = symbol::externals<uint16_t>(project->image->symbol_table->entries());
		//REQUIRE(boost::size(externs) == 1);
		//CHECK((*externs.begin())->name == "s");
	}

	SECTION("Valid .INPUT + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".INPUT s ;Hi guys\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 1);
		auto maybe_input = project->image->os->body_ir->ir_lines[0];
		auto as_input = std::dynamic_pointer_cast<masm::ir::dot_input<uint16_t> >(maybe_input);
		REQUIRE(as_input);
		REQUIRE(as_input->comment);
		CHECK(as_input->comment.value() == "Hi guys");

		// Check for externalized symbol definition.
		REQUIRE(symbol::exists<uint16_t>({project->image->symbol_table}, "s"));
		// TODO: Check that "s" is marked as global
	}

	SECTION("Invalid .INPUT + symbol + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		// TODO: Ban self-references on input, since input generates no object code.
		file->body = "s: .INPUT s ;Hi guys\n"; // Self reference is actually okay here, but has no use.
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);


	}

	SECTION("No dec in .INPUT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".INPUT 22\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

	SECTION("No hex in .INPUT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".INPUT 0xbeef\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No signed dec in .input") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".input -19\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No string in .INPUT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".INPUT \"HI\"\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
}
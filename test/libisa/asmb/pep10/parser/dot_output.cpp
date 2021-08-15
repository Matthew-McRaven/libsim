#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "asmb/pep10/directives.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot output", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("Valid .OUTPUT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:asra\n.OUTPUT s\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 2);
		auto maybe_output = project->image->os->body_ir->ir_lines[1];
		auto as_output = std::dynamic_pointer_cast<masm::ir::dot_output<uint16_t> >(maybe_output);
		REQUIRE(as_output);

		// Check for externalized symbol definition.
		REQUIRE(symbol::exists<uint16_t>({project->image->symbol_table}, "s"));
		// TODO: Check that "s" is marked as global
		// TODO: Check that there is only one external symbol.
		// TODO: Check that the list of external symbols starts with a symbol named "s".
		//auto externs = symbol::externals<uint16_t>(project->image->symbol_table->entries());
		//REQUIRE(boost::size(externs) == 1);
		//CHECK((*externs.begin())->name == "s");
	}

	SECTION("Valid .OUTPUT + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".OUTPUT s ;Hi guys\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 1);
		auto maybe_output = project->image->os->body_ir->ir_lines[0];
		auto as_output = std::dynamic_pointer_cast<masm::ir::dot_output<uint16_t> >(maybe_output);
		REQUIRE(as_output);
		REQUIRE(as_output->comment);
		CHECK(as_output->comment.value() == "Hi guys");

		// Check for externalized symbol definition.
		REQUIRE(symbol::exists<uint16_t>({project->image->symbol_table}, "s"));
		// TODO: Check that "s" is marked as global
	}

	SECTION("Invalid .OUTPUT + symbol + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		// TODO: Ban self-references on output, since output generates no object code.
		file->body = "s: .OUTPUT s ;Hi guys\n"; // Self reference is actually okay here, but has no use.
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);


	}

	SECTION("No dec in .OUTPUT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".OUTPUT 22\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

	SECTION("No hex in .OUTPUT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".OUTPUT 0xbeef\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No signed dec in .output") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".output -19\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No string in .OUTPUT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".OUTPUT \"HI\"\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
}
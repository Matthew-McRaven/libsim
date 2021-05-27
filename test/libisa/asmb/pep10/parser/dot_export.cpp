#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "asmb/pep10/directives.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot export", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("Valid .EXPORT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:asra\n.EXPORT s\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->section;
		REQUIRE(project->images[0]->section->body_ir->ir_lines.size() == 2);
		auto maybe_export = project->images[0]->section->body_ir->ir_lines[1];
		auto as_export = std::dynamic_pointer_cast<asmb::pep10::dot_export<uint16_t> >(maybe_export);
		REQUIRE(as_export);

		// Check for externalized symbol definition.
		REQUIRE(symbol::exists<uint16_t>("s", {project->images[0]->symbol_table}));
		// TODO: Check that "s" is marked as global
		// TODO: Check that there is only one external symbol.
		// TODO: Check that the list of external symbols starts with a symbol named "s".
		//auto externs = symbol::externals<uint16_t>(project->images[0]->symbol_table->entries());
		//REQUIRE(boost::size(externs) == 1);
		//CHECK((*externs.begin())->name == "s");
	}

	SECTION("Valid .EXPORT + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".EXPORT s ;Hi guys\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->section;
		REQUIRE(project->images[0]->section->body_ir->ir_lines.size() == 1);
		auto maybe_export = project->images[0]->section->body_ir->ir_lines[0];
		auto as_export = std::dynamic_pointer_cast<asmb::pep10::dot_export<uint16_t> >(maybe_export);
		REQUIRE(as_export);
		REQUIRE(as_export->comment);
		CHECK(as_export->comment.value() == "Hi guys");

		// Check for externalized symbol definition.
		REQUIRE(symbol::exists<uint16_t>("s", {project->images[0]->symbol_table}));
		// TODO: Check that "s" is marked as global
	}

	SECTION("Invalid .EXPORT + symbol + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		// TODO: Ban self-references on EXPORT, since EXPORT generates no object code.
		file->body = "s: .EXPORT s ;Hi guys\n"; // Self reference is actually okay here, but has no use.
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);


	}

	SECTION("No dec in .EXPORT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".EXPORT 22\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

	SECTION("No hex in .EXPORT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".EXPORT 0xbeef\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No signed dec in .EXPORT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".EXPORT -19\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No string in .EXPORT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".EXPORT \"HI\"\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
}
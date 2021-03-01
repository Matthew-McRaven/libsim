#include "catch.hpp"
#include "helper.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/directives.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot export", "[asmb::pep10::parser]"  ) {

	auto driver = make_driver();

	SECTION("Valid .EXPORT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:asra\n.EXPORT s\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 2);
		auto maybe_export = project->images[0]->sections[0]->body_ir->ir_lines[1];
		auto as_export = std::dynamic_pointer_cast<asmb::pep10::dot_export<uint16_t> >(maybe_export);
		REQUIRE(as_export);

		// Check for externalized symbol definition.
		REQUIRE(project->symbol_table->exists("s"));
		auto externs = project->symbol_table->getExternalSymbols();
		REQUIRE(externs.size() == 1);
		CHECK(externs[0]->getName() == "s");
	}

	SECTION("Valid .EXPORT + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".EXPORT s ;Hi guys\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 1);
		auto maybe_export = project->images[0]->sections[0]->body_ir->ir_lines[0];
		auto as_export = std::dynamic_pointer_cast<asmb::pep10::dot_export<uint16_t> >(maybe_export);
		REQUIRE(as_export);
		REQUIRE(as_export->comment);
		CHECK(as_export->comment.value() == "Hi guys");

		// Check for externalized symbol definition.
		REQUIRE(project->symbol_table->exists("s"));
		auto externs = project->symbol_table->getExternalSymbols();
		REQUIRE(externs.size() == 1);
		CHECK(externs[0]->getName() == "s");
	}

	SECTION("Invalid .EXPORT + symbol + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		// TODO: Ban self-references on EXPORT, since EXPORT generates no object code.
		file->body = "s: .EXPORT s ;Hi guys\n"; // Self reference is actually okay here, but has no use.
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);


	}

	SECTION("No dec in .EXPORT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".EXPORT 22\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

	SECTION("No hex in .EXPORT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".EXPORT 0xbeef\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No signed dec in .EXPORT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".EXPORT -19\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No string in .EXPORT") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".EXPORT \"HI\"\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
}
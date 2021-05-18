#include "catch.hpp"
#include "helper.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/directives.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot USCALL", "[asmb::pep10::parser]"  ) {

	auto driver = make_driver();

	SECTION("Valid .USCALL") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:asra\n.USCALL s\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 2);
		auto maybe_USCALL = project->images[0]->sections[0]->body_ir->ir_lines[1];
		auto as_USCALL = std::dynamic_pointer_cast<asmb::pep10::dot_uscall<uint16_t> >(maybe_USCALL);
		REQUIRE(as_USCALL);

		// Check for externalized symbol definition.
		REQUIRE(project->images[0]->symbol_table->exists("s"));
		REQUIRE(project->macro_registry->contains("s"));
	}

	SECTION("Valid .USCALL + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".USCALL s ;Hi guys\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 1);
		auto maybe_USCALL = project->images[0]->sections[0]->body_ir->ir_lines[0];
		auto as_USCALL = std::dynamic_pointer_cast<asmb::pep10::dot_uscall<uint16_t> >(maybe_USCALL);
		REQUIRE(as_USCALL);
		REQUIRE(as_USCALL->comment);
		CHECK(as_USCALL->comment.value() == "Hi guys");

		// Check for externalized symbol definition.
		REQUIRE(project->images[0]->symbol_table->exists("s"));
		REQUIRE(project->macro_registry->contains("s"));
	}

	SECTION("Invalid .USCALL + symbol + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		// TODO: Ban self-references on.USCALL, since.USCALL generates no object code.
		file->body = "s: .USCALL s ;Hi guys\n"; // Self reference is actually okay here, but has no use.
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);


	}

	SECTION("No dec in .USCALL") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".USCALL 22\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

	SECTION("No hex in .USCALL") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".USCALL 0xbeef\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No signed dec in .USCALL") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".USCALL -19\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No string in .USCALL") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".USCALL \"HI\"\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
}
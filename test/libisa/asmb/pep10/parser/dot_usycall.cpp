#include "catch.hpp"
#include "helper.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/directives.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot usycall", "[asmb::pep10::parser]"  ) {

	auto driver = make_driver();

	SECTION("Valid .USYCALL") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:asra\n.USYCALL s\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 2);
		auto maybe_usycall = project->images[0]->sections[0]->body_ir->ir_lines[1];
		auto as_usycall = std::dynamic_pointer_cast<asmb::pep10::dot_usycall<uint16_t> >(maybe_usycall);
		REQUIRE(as_usycall);

		// Check for externalized symbol definition.
		REQUIRE(project->symbol_table->exists("s"));
		REQUIRE(project->macro_registry->contains("s"));
	}

	SECTION("Valid .USYCALL + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".USYCALL s ;Hi guys\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 1);
		auto maybe_usycall = project->images[0]->sections[0]->body_ir->ir_lines[0];
		auto as_usycall = std::dynamic_pointer_cast<asmb::pep10::dot_usycall<uint16_t> >(maybe_usycall);
		REQUIRE(as_usycall);
		REQUIRE(as_usycall->comment);
		CHECK(as_usycall->comment.value() == "Hi guys");

		// Check for externalized symbol definition.
		REQUIRE(project->symbol_table->exists("s"));
		REQUIRE(project->macro_registry->contains("s"));
	}

	SECTION("Invalid .USYCALL + symbol + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		// TODO: Ban self-references on.USYCALL, since.USYCALL generates no object code.
		file->body = "s: .USYCALL s ;Hi guys\n"; // Self reference is actually okay here, but has no use.
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);


	}

	SECTION("No dec in .USYCALL") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".USYCALL 22\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

	SECTION("No hex in .USYCALL") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".USYCALL 0xbeef\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No signed dec in .USYCALL") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".USYCALL -19\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No string in .USYCALL") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".USYCALL \"HI\"\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
}
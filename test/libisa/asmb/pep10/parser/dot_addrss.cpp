#include "catch.hpp"
#include "helper.hpp"

#include "asmb/pep10/ir.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot addrss", "[asmb::pep10::parser]"  ) {

	auto driver = make_driver();

	SECTION("Valid .ADDRSS") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:asra\n.ADDRSS s\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 2);
		auto maybe_address = project->images[0]->sections[0]->body_ir->ir_lines[1];
		auto as_address = std::dynamic_pointer_cast<masm::ir::dot_address<uint16_t> >(maybe_address);
		REQUIRE(as_address);
	}

	SECTION("Valid .ADDRSS + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".ADDRSS s ;Hi guys\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 1);
		auto maybe_address = project->images[0]->sections[0]->body_ir->ir_lines[0];
		auto as_address = std::dynamic_pointer_cast<masm::ir::dot_address<uint16_t> >(maybe_address);
		REQUIRE(as_address);
		REQUIRE(as_address->comment);
		CHECK(as_address->comment.value() == "Hi guys");
	}

	SECTION("Valid .ADDRSS + symbol + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s: .ADDRSS s ;Hi guys\n"; // Self reference is actually okay here, but has no use.
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 1);
		auto maybe_address = project->images[0]->sections[0]->body_ir->ir_lines[0];
		auto as_address = std::dynamic_pointer_cast<masm::ir::dot_address<uint16_t> >(maybe_address);
		REQUIRE(as_address);
		REQUIRE(as_address->comment);
		REQUIRE(as_address->symbol_entry);
		CHECK(as_address->symbol_entry->getName() == "s");
	}

	SECTION("No dec in .ADDRSS") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".ADDRSS 22\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

	SECTION("No hex in .ADDRSS") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".ADDRSS 0xbeef\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No signed dec in .ADDRSS") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".ADDRSS -19\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No string in .ADDRSS") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".ADDRSS \"HI\"\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
}
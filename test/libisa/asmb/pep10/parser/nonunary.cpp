#include "catch.hpp"
#include "helper.hpp"

#include "asmb/pep10/ir.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse nonunary instructions", "[asmb::pep10::parser]"  ) {

	auto driver = make_driver();

	SECTION("Nonunary instruction, requires addressing mode.") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "ADDA 20, i\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 2);
		auto maybe_nonunary = project->images[0]->sections[0]->body_ir->ir_lines[0];
		auto as_nonunary = std::dynamic_pointer_cast<asmb::pep10::nonunary_instruction>(maybe_nonunary);
		REQUIRE(as_nonunary);
		CHECK(as_nonunary->mnemonic == isa::pep10::instruction_mnemonic::ADDA);
	}

	SECTION("Nonunary instruction, branches.") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "BR 0x20,i\nBR 0x20,x\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 2);
		auto maybe_nonunary = project->images[0]->sections[0]->body_ir->ir_lines[0];
		auto as_nonunary = std::dynamic_pointer_cast<asmb::pep10::nonunary_instruction>(maybe_nonunary);
	}

	SECTION("Nonunary instruction, implict addressing mode.") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "BR 20\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 1);
		auto maybe_nonunary = project->images[0]->sections[0]->body_ir->ir_lines[0];
		auto as_nonunary = std::dynamic_pointer_cast<asmb::pep10::nonunary_instruction>(maybe_nonunary);
		REQUIRE(as_nonunary);
		CHECK(as_nonunary->mnemonic == isa::pep10::instruction_mnemonic::BR);
		CHECK(as_nonunary->addressing_mode == isa::pep10::addressing_mode::I);
	}

	SECTION("Nonunary instruction, illegal addr mode.") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "LDWT 20, sfx\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

}
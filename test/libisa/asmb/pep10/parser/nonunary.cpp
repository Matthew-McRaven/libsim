#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse nonunary instructions", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("Nonunary instruction, requires addressing mode.") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "ADDA 20, i\n.END\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 2);
		auto maybe_nonunary = project->image->os->body_ir->ir_lines[0];
		auto as_nonunary = std::dynamic_pointer_cast<asmb::pep10::nonunary_instruction>(maybe_nonunary);
		REQUIRE(as_nonunary);
		CHECK(as_nonunary->mnemonic == isa::pep10::instruction_mnemonic::ADDA);
	}

	SECTION("Nonunary instruction, branches.") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "BR 0x20,i\nBR 0x20,x\n.END\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 3);
		auto maybe_nonunary = project->image->os->body_ir->ir_lines[0];
		auto as_nonunary = std::dynamic_pointer_cast<asmb::pep10::nonunary_instruction>(maybe_nonunary);
	}

	SECTION("Nonunary instruction, implict addressing mode.") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "BR 20\n.END\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 2);
		auto maybe_nonunary = project->image->os->body_ir->ir_lines[0];
		auto as_nonunary = std::dynamic_pointer_cast<asmb::pep10::nonunary_instruction>(maybe_nonunary);
		REQUIRE(as_nonunary);
		CHECK(as_nonunary->mnemonic == isa::pep10::instruction_mnemonic::BR);
		CHECK(as_nonunary->addressing_mode == isa::pep10::addressing_mode::I);
	}

	SECTION("Nonunary instruction, illegal addr mode.") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "LDWT 20, sfx\n.END\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

}
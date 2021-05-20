#include "catch.hpp"
#include "helper.hpp"

#include "asmb/pep10/ir.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot word", "[asmb::pep10::parser]"  ) {

	auto driver = make_driver();

	SECTION("decimal .WORD") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".WORD 33\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->section;
		REQUIRE(project->images[0]->section->body_ir->ir_lines.size() == 1);
		auto maybe_word = project->images[0]->section->body_ir->ir_lines[0];
		auto as_word = std::dynamic_pointer_cast<masm::ir::dot_word<uint16_t> >(maybe_word);
		REQUIRE(as_word->argument->value() == 33);
	}
	
	SECTION("signed decimal .WORD") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".WORD -33\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->section;
		REQUIRE(project->images[0]->section->body_ir->ir_lines.size() == 1);
		auto maybe_word = project->images[0]->section->body_ir->ir_lines[0];
		auto as_word = std::dynamic_pointer_cast<masm::ir::dot_word<uint16_t> >(maybe_word);
		REQUIRE(as_word->argument->value() == static_cast<uint16_t>(-33));
	}

	SECTION("symbolic .WORD") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:.EQUATE 33\n.WORD s\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->section;
		REQUIRE(project->images[0]->section->body_ir->ir_lines.size() == 2);
		auto maybe_word = project->images[0]->section->body_ir->ir_lines[1];
		auto as_word = std::dynamic_pointer_cast<masm::ir::dot_word<uint16_t> >(maybe_word);
		// TODO: Check back after finishing equate.
		REQUIRE(as_word->argument->value() == 33);
	}

	SECTION("hex .WORD") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".WORD 0x21\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->section;
		REQUIRE(project->images[0]->section->body_ir->ir_lines.size() == 1);
		auto maybe_word = project->images[0]->section->body_ir->ir_lines[0];
		auto as_word = std::dynamic_pointer_cast<masm::ir::dot_word<uint16_t> >(maybe_word);
		REQUIRE(as_word->argument->value() == 33);
	}

	SECTION("char .WORD") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".WORD '!'\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->section;
		REQUIRE(project->images[0]->section->body_ir->ir_lines.size() == 1);
		auto maybe_word = project->images[0]->section->body_ir->ir_lines[0];
		auto as_word = std::dynamic_pointer_cast<masm::ir::dot_word<uint16_t> >(maybe_word);
		REQUIRE(as_word->argument->value() == 33);
	}

	SECTION("string .WORD") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".WORD \"!\"\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->section;
		REQUIRE(project->images[0]->section->body_ir->ir_lines.size() == 1);
		auto maybe_word = project->images[0]->section->body_ir->ir_lines[0];
		auto as_word = std::dynamic_pointer_cast<masm::ir::dot_word<uint16_t> >(maybe_word);
		REQUIRE(as_word->argument->value() == 33);
	}

	SECTION("No 3 byte argument") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".WORD \"!!!\"\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
}
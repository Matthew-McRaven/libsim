#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot equate", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("decimal .EQUATE") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:.EQUATE 33\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 1);
		auto maybe_word = project->image->os->body_ir->ir_lines[0];
		auto as_word = std::dynamic_pointer_cast<masm::ir::dot_equate<uint16_t> >(maybe_word);
		REQUIRE(as_word->argument->value() == 33);
	}
	
	SECTION("signed decimal .EQUATE") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:.EQUATE -33\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 1);
		auto maybe_word = project->image->os->body_ir->ir_lines[0];
		auto as_word = std::dynamic_pointer_cast<masm::ir::dot_equate<uint16_t> >(maybe_word);
		REQUIRE(as_word->argument->value() == static_cast<uint16_t>(-33));
	}

	SECTION("no symbolic .EQUATE") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:.EQUATE 33\nk:.EQUATE s\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

	SECTION("hex .EQUATE") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:.EQUATE 0x21\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 1);
		auto maybe_word = project->image->os->body_ir->ir_lines[0];
		auto as_word = std::dynamic_pointer_cast<masm::ir::dot_equate<uint16_t> >(maybe_word);
		REQUIRE(as_word->argument->value() == 33);
	}

	SECTION("char .EQUATE") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:.EQUATE '!'\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 1);
		auto maybe_word = project->image->os->body_ir->ir_lines[0];
		auto as_word = std::dynamic_pointer_cast<masm::ir::dot_equate<uint16_t> >(maybe_word);
		REQUIRE(as_word->argument->value() == 33);
	}

	SECTION("string .EQUATE") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:.EQUATE \"!\"\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 1);
		auto maybe_word = project->image->os->body_ir->ir_lines[0];
		auto as_word = std::dynamic_pointer_cast<masm::ir::dot_equate<uint16_t> >(maybe_word);
		REQUIRE(as_word->argument->value() == 33);
	}

	SECTION("No 3 byte argument") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:.EQUATE \"!!!\"\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
}
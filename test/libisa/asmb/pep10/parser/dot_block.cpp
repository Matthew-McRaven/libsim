#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot block", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("decimal .BLOCK") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".BLOCK 33\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 1);
		auto maybe_block = project->image->os->body_ir->ir_lines[0];
		auto as_block = std::dynamic_pointer_cast<masm::ir::dot_block<uint16_t> >(maybe_block);
		REQUIRE(as_block->object_code_bytes() == 33);
	}
	
	SECTION("signed decimal .BLOCK") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".BLOCK -33\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 1);
		auto maybe_block = project->image->os->body_ir->ir_lines[0];
		auto as_block = std::dynamic_pointer_cast<masm::ir::dot_block<uint16_t> >(maybe_block);
		REQUIRE(as_block->object_code_bytes() == static_cast<uint16_t>(-33));
	}

	SECTION("hex .BLOCK") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".BLOCK 0x21\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 1);
		auto maybe_block = project->image->os->body_ir->ir_lines[0];
		auto as_block = std::dynamic_pointer_cast<masm::ir::dot_block<uint16_t> >(maybe_block);
		REQUIRE(as_block->object_code_bytes() == 33);
	}

	SECTION("no char .BLOCK") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".BLOCK '!'\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

	SECTION("no string .BLOCK") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".BLOCK \"!\"\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);

	}
}
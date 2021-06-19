#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot burn", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("no decimal .BURN") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".BURN 33\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	
	SECTION("no signed decimal .BURN") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".BURN -33\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

	SECTION("hex .BURN") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".BURN 0x21\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(project->image->os->body_ir->ir_lines.size() == 1);
		auto maybe_burn = project->image->os->body_ir->ir_lines[0];
		auto as_burn = std::dynamic_pointer_cast<masm::ir::dot_burn<uint16_t> >(maybe_burn);
		REQUIRE(as_burn->argument->value() == 33);
	}

	SECTION("char .BURN") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".BURN '!'\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

	SECTION("no string .BURN") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".BURN \"!\"\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

}	
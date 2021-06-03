#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "masm/ir/directives.hpp"
#include "masm/ir/empty.hpp"

TEST_CASE( "Test for multiple / undefined symbols", "[masm::ir::assign_addr]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("Multiple defined symbol") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "os";
		file->body = ".BURN 0x0001\nbad:ASRA\nbad:NOTX\n.END\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::ADDRESS_ASSIGN);
		REQUIRE(!res.first);
	}
	SECTION("Undefined symbol") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "os";
		file->body = ".BURN 0x0002\nbr bad\n.END\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::ADDRESS_ASSIGN);
		REQUIRE(!res.first);
	}
}
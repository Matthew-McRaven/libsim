#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "masm/utils/listing.hpp"

TEST_CASE( "Test vector<uint8_t> => string conversion.", "[masm::utils]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("Check simple program") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file_os = std::make_shared<masm::project::source_file>();
		file_os->name = "os";
		file_os->body = ".BURN 0x2\n.WORD 65\n.BYTE 23\n.END\n";
		auto res = driver->assemble_os(project, file_os, masm::project::toolchain_stage::PACK);

		REQUIRE(res.first);
		auto x = project->image->os;
		REQUIRE(masm::utils::generate_formatted_bytecode(x) == "00 41 17 zz");
	}

}
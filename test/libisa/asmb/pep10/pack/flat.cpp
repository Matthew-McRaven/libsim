#include "catch.hpp"
#include "helper.hpp"

#include "asmb/pep10/ir.hpp"
#include "masm/ir/directives.hpp"
#include "masm/ir/empty.hpp"
#include "masm/elf/pack.hpp"

TEST_CASE( "Flatten simple programs to ELF binary.", "[masm::elf]"  ) {
	using namespace asmb::pep10::pack;
	auto driver = make_driver();

	SECTION("Unary instructions") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "ASRA\nNOTX\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::PACK);
		REQUIRE(res.first);
	}

	SECTION("Non-unary instructions") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "br main\nADDA 1,i\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::PACK);
		REQUIRE(res.first);
	}

	SECTION("Fixed width dot commands instructions") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".WORD 1\n.BYTE 2\n.WORD 0xffff\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::PACK);
		REQUIRE(res.first);
	}

	SECTION("Multiple ASCII commands") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "a:.ASCII \"hi\"\nb:.ASCII \"world\"\nc:.BYTE 2\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::PACK);
		REQUIRE(res.first);
	}
}
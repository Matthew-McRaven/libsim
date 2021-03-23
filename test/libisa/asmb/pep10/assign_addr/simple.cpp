#include "catch.hpp"
#include "helper.hpp"

#include "asmb/pep10/ir.hpp"
#include "masm/ir/directives.hpp"
#include "masm/ir/empty.hpp"

TEST_CASE( "Allocate address to simple programs", "[masm::ir::assign_addr]"  ) {
	using namespace asmb::pep10::assign_addr;
	auto driver = make_driver();

	SECTION("Unary instructions") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "ASRA\nNOTX\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::ADDRESS_ASSIGN);
		std::cout << res.second;
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 3);
		// Line 0.
		auto maybe_unary0 = project->images[0]->sections[0]->body_ir->ir_lines[0];
		auto as_unary0 = std::dynamic_pointer_cast<asmb::pep10::unary_instruction>(maybe_unary0);
		REQUIRE(as_unary0);
		CHECK(as_unary0->mnemonic == isa::pep10::instruction_mnemonic::ASRA);
		CHECK(!as_unary0->comment);
		CHECK(as_unary0->base_address() == 0);
		// Line 1
		auto maybe_unary1 = project->images[0]->sections[0]->body_ir->ir_lines[1];
		auto as_unary1 = std::dynamic_pointer_cast<asmb::pep10::unary_instruction>(maybe_unary1);
		REQUIRE(as_unary1);
		CHECK(as_unary1->mnemonic == isa::pep10::instruction_mnemonic::NOTX);
		CHECK(!as_unary1->comment);
		CHECK(as_unary1->base_address() == 1);
	}
}
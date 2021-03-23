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
		file->body = "ASRA\nASRA\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver->assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 3);
		auto maybe_unary = project->images[0]->sections[0]->body_ir->ir_lines[0];
		auto as_unary = std::dynamic_pointer_cast<asmb::pep10::unary_instruction>(maybe_unary);
		REQUIRE(as_unary);
		CHECK(as_unary->mnemonic == isa::pep10::instruction_mnemonic::ASRA);
		CHECK(!as_unary->comment);;
	}
}
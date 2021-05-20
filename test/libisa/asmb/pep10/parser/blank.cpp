#include "catch.hpp"
#include "helper.hpp"

#include "asmb/pep10/ir.hpp"
#include "masm/ir/directives.hpp"
#include "masm/ir/empty.hpp"

TEST_CASE( "Parse blank line", "[asmb::pep10::parser]"  ) {

	auto driver = make_driver();

	SECTION("Blank line") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "\n\n";
		auto res = driver->assemble_project(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->section;
		REQUIRE(project->images[0]->section->body_ir->ir_lines.size() == 2);
		auto maybe_empty = project->images[0]->section->body_ir->ir_lines[0];
		auto as_empty = std::dynamic_pointer_cast<masm::ir::blank_line<uint16_t> >(maybe_empty);
		REQUIRE(as_empty);
	}
}
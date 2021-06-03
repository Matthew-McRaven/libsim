#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot end", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();
	
	SECTION("Lonely End.") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".END\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->os;
		REQUIRE(project->images[0]->os->body_ir->ir_lines.size() == 1);
		auto maybe_end = project->images[0]->os->body_ir->ir_lines[0];
		auto as_end = std::dynamic_pointer_cast<masm::ir::dot_end<uint16_t> >(maybe_end);
		REQUIRE(as_end);
	}
}
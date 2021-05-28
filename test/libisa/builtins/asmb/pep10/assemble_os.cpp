#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "ex_registry.hpp"
#include "masm/ir/directives.hpp"
#include "masm/ir/macro.hpp"

TEST_CASE( "Parse entire OS", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("Assemble OS.") {	
		auto project = masm::project::init_project<uint16_t>();
		auto ex = registry::instance();
		for(const auto& macro : ex.macros()){
			CHECK(project->macro_registry->register_macro(macro.name, macro.text, masm::MacroType::CoreMacro));
		}
		auto fig_os = ex.find("pep10", 9, 0).value();
		auto text_os = fig_os.elements.at(element_type::kPep);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "os";
		file->body = text_os;
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::PACK);

		REQUIRE(res.first);
		auto x = project->images[0]->os;
		REQUIRE(project->images[0]->os->body_ir->ir_lines.size() == 642);
	}

}
#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "ex_registry.hpp"
#include "masm/ir/directives.hpp"
#include "masm/ir/macro.hpp"

TEST_CASE( "Parse & pack all sample programs", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();
	auto ex = registry::instance();
	auto fig_os = ex.find("pep10", 9, "00").value();
	auto text_os = fig_os.elements.at(element_type::kPep);
	auto file_os = std::make_shared<masm::project::source_file>();
	file_os->name = "os";
	file_os->body = text_os;
	for (const auto& figure : ex.figures()) {
		if(figure.elements.find(element_type::kPep) == figure.elements.end()) continue;
		if(figure.chapter >= 8) continue;
		DYNAMIC_SECTION( fmt::format("Assembling fig {}.{}", figure.chapter, figure.fig)) {
			auto project = masm::project::init_project<uint16_t>();
			for(const auto& macro : ex.macros()) project->macro_registry->register_macro(macro.name, macro.text, masm::MacroType::CoreMacro);
			auto file_user = std::make_shared<masm::project::source_file>();
			file_user->name = "user";
			file_user->body = figure.elements.at(element_type::kPep);
			auto res = driver->assemble_joint(project, file_os, file_user, masm::project::toolchain_stage::PACK);
			CHECK(res.first);
			if(!res.first) std::cout << fmt::format("Figure {}.{}", figure.chapter, figure.fig) << std::endl;
		}
	}

}
#include "catch.hpp"
#include <elfio/elfio.hpp>

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "ex_registry.hpp"
#include "masm/ir/directives.hpp"
#include "masm/ir/macro.hpp"

std::shared_ptr<ELFIO::elfio> os_to_image()
{
	using namespace asmb::pep10::driver;
	auto driver = make_driver();
	auto project = masm::project::init_project<uint16_t>();
	auto ex = registry::instance();
	for(const auto& macro : ex.macros()){
		CHECK(project->macro_registry->register_macro(macro.name, macro.text, masm::MacroType::CoreMacro));
	}
	auto fig_os = ex.find("pep10", 9, "00").value();
	auto text_os = fig_os.elements.at(element_type::kPep);
	auto file_os = std::make_shared<masm::project::source_file>();
	file_os->name = "os";
	file_os->body = text_os;
	auto res = driver->assemble_os(project, file_os, masm::project::toolchain_stage::FINISHED);
	std::cout << res.second;
	REQUIRE(res.first);
	REQUIRE(project->as_elf);
	return project->as_elf;
	
}
TEST_CASE( "Convert ELF image to Pep/10 machine", "[elf_tools::pep10]"  ) {


	SECTION("Assemble OS.") {	
		auto image = os_to_image();
	}

}
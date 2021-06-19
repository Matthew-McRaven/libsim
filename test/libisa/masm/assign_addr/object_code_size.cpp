#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "ex_registry.hpp"
#include "masm/ir/directives.hpp"
#include "masm/ir/macro.hpp"

TEST_CASE( "Check that reported byte length matches actual byte length.", "[asmb::pep10::addr_assign]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("Check OS length.") {	
		auto project = masm::project::init_project<uint16_t>();
		auto ex = registry::instance();
		for(const auto& macro : ex.macros()){
			CHECK(project->macro_registry->register_macro(macro.name, macro.text, masm::MacroType::CoreMacro));
		}
		auto fig_os = ex.find("pep10", 9, "00").value();
		auto text_os = fig_os.elements.at(element_type::kPep);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "os";
		file->body = text_os;
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::PACK);

		REQUIRE(res.first);
		for(const auto& line : project->image->os->body_ir->ir_lines)
		{
			std::vector<uint8_t> bytes;
			line->append_object_code(bytes);
			if(line->emits_object_code) CHECK(line->object_code_bytes() == bytes.size());
			if(line->emits_object_code && (line->object_code_bytes() != bytes.size())) {
				std::cout << "Damn";
			}
		}
	}
}
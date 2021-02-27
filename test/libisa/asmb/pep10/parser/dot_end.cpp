#include "catch.hpp"

#include "masm/frontend/tokens.hpp"
#include "masm/frontend/tokenizer.hpp"
#include "asmb/pep10/tokenizer.hpp"
#include "asmb/pep10/parser.hpp"
#include "asmb/pep10/ir.hpp"
#include "masm/registry.hpp"
#include "masm/project/project.hpp"
#include "masm/project/init_project.hpp"
#include "masm/driver.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot end", "[asmb::pep10::parser]"  ) {
    
	using tokenizer_t = asmb::pep10::tokenizer<masm::frontend::lexer_t>;
	using stage_t = masm::project::toolchain_stage;
	using driver_t = masm::driver<uint16_t, stage_t>;
	using project_t = driver_t::project_t;
	using section_t = driver_t::section_t;
	using result_t = driver_t::result_t;
	using transform_t = driver_t::transform_t;
	
	driver_t driver;

	masm::frontend::tokenizer<uint16_t, tokenizer_t> tokenizer{};
	transform_t tx_tokenizer = [&](project_t& proj, std::list<driver_t::work_t>& work) {
		bool success = true;
		driver_t::work_iterable_t result_work;
		std::transform(work.begin(), work.end(), std::back_inserter(result_work),[&](auto& value){
				success &= tokenizer.tokenize(proj, std::get<driver_t::section_t>(value));
				return driver_t::work_iterable_t::value_type{stage_t::TOKEN, value};
			});
		return driver_t::result_t{success, result_work};
	};
	driver.register_transform(tx_tokenizer, stage_t::RAW);

	masm::frontend::preprocessor<uint16_t, tokenizer_t> preproc{};
	transform_t tx_preproc = [&](project_t& proj, std::list<driver_t::work_t>& work) {
		bool success = true;
		driver_t::work_iterable_t result_work;
		for(auto value : work) {
			auto [local_success, local_children] = preproc.preprocess(proj, std::get<driver_t::section_t>(value));
			for(auto child : local_children) {
				result_work.emplace_back(driver_t::work_iterable_t::value_type{stage_t::RAW, 
					std::static_pointer_cast<masm::elf::code_section<uint16_t>>(child)});
			}
			result_work.emplace_back(driver_t::work_iterable_t::value_type{stage_t::PREPROCESS, value});
			success &= local_success;
		}
		return driver_t::result_t{success, result_work};
	};
	driver.register_transform(tx_preproc, stage_t::TOKEN);

	asmb::pep10::parser parser{};
	transform_t tx_parser = [&](project_t& proj, std::list<driver_t::work_t>& work) {
		bool success = true;
		driver_t::work_iterable_t result_work;
		std::transform(work.begin(), work.end(), std::back_inserter(result_work),[&](auto& value){
				success &= parser.parse(proj, std::get<driver_t::section_t>(value));
				return driver_t::work_iterable_t::value_type{stage_t::SYMANTIC, value};
			});
		return driver_t::result_t{success, result_work};
	};
	driver.register_transform(tx_parser, stage_t::PREPROCESS);

	SECTION("Lonely End.") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->sections[0];
		REQUIRE(project->images[0]->sections[0]->body_ir->ir_lines.size() == 1);
		auto maybe_end = project->images[0]->sections[0]->body_ir->ir_lines[0];
		auto as_end = std::dynamic_pointer_cast<masm::ir::dot_end<uint16_t> >(maybe_end);
		REQUIRE(as_end);
	}
}
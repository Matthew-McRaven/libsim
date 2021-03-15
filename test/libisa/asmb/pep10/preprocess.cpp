#include "catch.hpp"

#include "masm/frontend/tokens.hpp"
#include "masm/frontend/tokenizer.hpp"
#include "asmb/pep10/tokenizer.hpp"
#include "masm/registry.hpp"
#include "masm/project/project.hpp"
#include "masm/project/init_project.hpp"
#include "masm/driver.hpp"

TEST_CASE( "Recognize existing macros", "[asmb::pep10::preprocessor]"  ) {
    
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

	SECTION("Invoke 0-arity macro.") {
		auto project = masm::project::init_project<uint16_t>();
		project->macro_registry->register_macro("HELLO0", "@HELLO0 0\n.END\n", masm::MacroType::CoreMacro);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "LWDA 20,d\n@HELLO0\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::PREPROCESS);
		CHECK(res.first);
	}
	SECTION("Invoke 1-arty macro with dec constant.") {	
		auto project = masm::project::init_project<uint16_t>();
		project->macro_registry->register_macro("HELLO1", "@HELLO1 1\n.END\n", masm::MacroType::CoreMacro);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "@HELLO1 01\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::PREPROCESS);
		CHECK(res.first);
	}
	SECTION("Invoke 1-arty macro with hex constant.") {	
		auto project = masm::project::init_project<uint16_t>();
		project->macro_registry->register_macro("HELLO1", "@HELLO1 1\n.END\n", masm::MacroType::CoreMacro);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "@HELLO1 0x01\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::PREPROCESS);
		CHECK(res.first);
	}
	SECTION("Invoke 1-arty macro with string constant.") {	
		auto project = masm::project::init_project<uint16_t>();
		project->macro_registry->register_macro("HELLO1", "@HELLO1 1\n.END\n", masm::MacroType::CoreMacro);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "@HELLO1 \"01\"\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::PREPROCESS);
		CHECK(res.first);
	}
	SECTION("Invoke 1-arty macro with character constant.") {	
		auto project = masm::project::init_project<uint16_t>();
		project->macro_registry->register_macro("HELLO1", "@HELLO1 1\n.END\n", masm::MacroType::CoreMacro);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "@HELLO1 '0'\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::PREPROCESS);
		CHECK(res.first);
	}
	SECTION("Invoke 1-arity macro with identifier.") {	
		auto project = masm::project::init_project<uint16_t>();
		project->macro_registry->register_macro("HELLO1", "@HELLO1 1\n.END\n", masm::MacroType::CoreMacro);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "LWDA 20,d\n@HELLO1 hello\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::PREPROCESS);
		CHECK(res.first);
	}

	SECTION("No such macro.") {
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "LWDA 20,d\n@HELLO1 hello,world\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::PREPROCESS);
		CHECK_FALSE(res.first);
		CHECK(!res.second.empty());
		auto section = std::static_pointer_cast<masm::elf::code_section<uint16_t> >(project->images[0]->sections[0]);
		auto errors = project->message_resolver->errors_for_section(section);
		REQUIRE(errors.size() == 1);
		CHECK(std::get<1>(*errors.begin()).message == fmt::format(masm::frontend::detail::error_does_not_exist, "HELLO1"));
	}

	SECTION("Invoke 1-arity macro with 2 args.") {	
		auto project = masm::project::init_project<uint16_t>();
		project->macro_registry->register_macro("HELLO1", "@HELLO1 1\n.END\n", masm::MacroType::CoreMacro);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "LWDA 20,d\n@HELLO1 hello,world\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::PREPROCESS);
		CHECK_FALSE(res.first);
		CHECK(!res.second.empty());
		auto section = std::static_pointer_cast<masm::elf::code_section<uint16_t> >(project->images[0]->sections[0]);
		auto errors = project->message_resolver->errors_for_section(section);
		REQUIRE(errors.size() == 1);
		CHECK(std::get<1>(*errors.begin()).message == fmt::format(masm::frontend::detail::error_bad_arg_count, 2, 1));
	}

	SECTION("Invoke 1-arity macro with 0 args.") {	
		auto project = masm::project::init_project<uint16_t>();
		project->macro_registry->register_macro("HELLO1", "@HELLO1 1\n.END\n", masm::MacroType::CoreMacro);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "LWDA 20,d\n@HELLO1\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::PREPROCESS);
		CHECK_FALSE(res.first);
		CHECK(!res.second.empty());
		auto section = std::static_pointer_cast<masm::elf::code_section<uint16_t> >(project->images[0]->sections[0]);
		auto errors = project->message_resolver->errors_for_section(section);
		REQUIRE(errors.size() == 1);
		CHECK(std::get<1>(*errors.begin()).message == fmt::format(masm::frontend::detail::error_bad_arg_count, 0, 1));
	}

	SECTION("1-macro inclusion loop") {	
		auto project = masm::project::init_project<uint16_t>();
		project->macro_registry->register_macro("HELLOA", "@HELLOA 0\n@HELLOA\n.END\n", masm::MacroType::CoreMacro);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "LWDA 20,d\n@HELLOA\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::PREPROCESS);
		CHECK_FALSE(res.first);
		auto section = std::static_pointer_cast<masm::elf::code_section<uint16_t> >(project->images[0]->sections[0]);
		auto errors = project->message_resolver->errors_for_section(section, true);
		REQUIRE(errors.size() == 1);
		CHECK(std::get<1>(*errors.begin()).message == masm::frontend::detail::error_circular_include);	
	}

	SECTION("2-macro inclusion loop") {	
		auto project = masm::project::init_project<uint16_t>();
		project->macro_registry->register_macro("HELLOA", "@HELLOA 0\n@HELLOB\n.END\n", masm::MacroType::CoreMacro);
		project->macro_registry->register_macro("HELLOB", "@HELLOB 0\n@HELLOA\n.END\n", masm::MacroType::CoreMacro);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "LWDA 20,d\n@HELLOA\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::PREPROCESS);
		CHECK_FALSE(res.first);
		auto section = std::static_pointer_cast<masm::elf::code_section<uint16_t> >(project->images[0]->sections[0]);
		auto errors = project->message_resolver->errors_for_section(section, true);
		REQUIRE(errors.size() == 1);
		CHECK(std::get<1>(*errors.begin()).message == masm::frontend::detail::error_circular_include);	
	}

	SECTION("4-macro inclusion loop") {	
		auto project = masm::project::init_project<uint16_t>();
		project->macro_registry->register_macro("HELLOA", "@HELLOA 0\n@HELLOB\n.END\n", masm::MacroType::CoreMacro);
		project->macro_registry->register_macro("HELLOB", "@HELLOB 0\n@HELLOC\n.END\n", masm::MacroType::CoreMacro);
		project->macro_registry->register_macro("HELLOC", "@HELLOC 0\n@HELLOD\n.END\n", masm::MacroType::CoreMacro);
		project->macro_registry->register_macro("HELLOD", "@HELLOD 0\n@HELLOA\n.END\n", masm::MacroType::CoreMacro);
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "LWDA 20,d\n@HELLOA\n.END\n";
		std::vector<driver_t::source_t> vec = {file};
		auto res = driver.assemble_project(project, vec, masm::project::toolchain_stage::PREPROCESS);
		CHECK_FALSE(res.first);
		auto section = std::static_pointer_cast<masm::elf::code_section<uint16_t> >(project->images[0]->sections[0]);
		auto errors = project->message_resolver->errors_for_section(section, true);
		REQUIRE(errors.size() == 1);
		CHECK(std::get<1>(*errors.begin()).message == masm::frontend::detail::error_circular_include);	
	}





}
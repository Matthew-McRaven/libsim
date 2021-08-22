#include "create_driver.hpp"

#include "ex_registry.hpp"
#include "masm/backend/sanity.hpp"
#include "masm/project/project.hpp"

std::shared_ptr<asmb::pep10::driver::driver_t> asmb::pep10::driver::make_driver()
{
	using namespace asmb::pep10::driver;
	auto driver = std::make_shared<driver_t>();

	auto tokenizer = std::make_shared<masm::frontend::tokenizer<uint16_t, tokenizer_t> >();
	transform_t tx_tokenizer = [=](project_t& proj, std::list<driver_t::work_t>& work) {
		bool success = true;
		driver_t::work_iterable_t result_work;
		std::transform(work.begin(), work.end(), std::back_inserter(result_work),[&](auto& value){
				success &= tokenizer->tokenize(proj, std::get<driver_t::section_t>(value));
				return driver_t::work_iterable_t::value_type{stage_t::TOKEN, value};
			});
		return driver_t::result_t{success, result_work};
	};
	driver->register_transform(tx_tokenizer, stage_t::RAW);

	auto preproc = std::make_shared<masm::frontend::preprocessor<uint16_t, tokenizer_t> >();
	transform_t tx_preproc = [=](project_t& proj, std::list<driver_t::work_t>& work) {
		bool success = true;
		driver_t::work_iterable_t result_work;
		for(auto value : work) {
			auto [local_success, local_children] = preproc->preprocess(proj, std::get<driver_t::section_t>(value));
			for(const auto& child : local_children) {
				result_work.emplace_back(driver_t::work_iterable_t::value_type{stage_t::RAW, 
					std::static_pointer_cast<masm::elf::code_section<uint16_t>>(child)});
			}
			result_work.emplace_back(driver_t::work_iterable_t::value_type{stage_t::PREPROCESS, value});
			success &= local_success;
		}
		return driver_t::result_t{success, result_work};
	};
	driver->register_transform(tx_preproc, stage_t::TOKEN);

	auto parser = std::make_shared<asmb::pep10::parser>();
	transform_t tx_parser = [=](project_t& proj, std::list<driver_t::work_t>& work) {
		bool success = true;
		driver_t::work_iterable_t result_work;
		std::transform(work.begin(), work.end(), std::back_inserter(result_work),[&](auto& value){
				success &= parser->parse(proj, std::get<driver_t::section_t>(value));
				return driver_t::work_iterable_t::value_type{stage_t::SYMANTIC, value};
			});
		return driver_t::result_t{success, result_work};
	};
	driver->register_transform(tx_parser, stage_t::PREPROCESS);

	transform_t tx_sanity1 = [=](project_t& proj, std::list<driver_t::work_t>& work) {
		bool success = true;
		driver_t::work_iterable_t result_work;
		using tls_t = std::shared_ptr<masm::elf::code_section<uint16_t>>;
		std::map<std::pair<tls_t,tls_t>, bool> parsed_tls;
		std::transform(work.begin(), work.end(), std::back_inserter(result_work),[&](auto& value){
				auto containing_image = std::get<driver_t::section_t>(value)->containing_image.lock();
				std::pair<tls_t, tls_t> tls = {containing_image->os, containing_image->user};
				if(parsed_tls.find(tls) == parsed_tls.end()) {
					bool val = whole_program_sanity_fixup<uint16_t>(proj, tls.first);
					if(tls.second) val &= whole_program_sanity_fixup<uint16_t>(proj, tls.second);
					success &= parsed_tls[tls] = val;
				}
				return driver_t::work_iterable_t::value_type{stage_t::WHOLE_PROGRAM_SANITY, value};
			});
		return driver_t::result_t{success, result_work};
	};
	driver->register_transform(tx_sanity1, stage_t::SYMANTIC);

	transform_t tx_addr = [=](project_t& proj, std::list<driver_t::work_t>& work) {
		bool success = true;
		std::set<std::shared_ptr<masm::elf::image<uint16_t>>> images;
		for(auto& value : work) {
			images.insert(std::get<driver_t::section_t>(value)->containing_image.lock());
		}

		driver_t::work_iterable_t result_work;
		std::transform(images.begin(), images.end(), std::back_inserter(result_work),[&](auto image){
			// Clang bug prevented success from being mutated directly ??
			auto help = masm::backend::assign_image(proj, image);
			success &= help;
			return driver_t::work_iterable_t::value_type{stage_t::ADDRESS_ASSIGN, image};
		});
		return driver_t::result_t{success, result_work};
	};
	driver->register_transform(tx_addr, stage_t::WHOLE_PROGRAM_SANITY);

	transform_t tx_elf = [=](project_t& proj, std::list<driver_t::work_t>& work) {
		bool success = true;
		driver_t::work_iterable_t result_work;
		for(auto value : work) {
			if(proj->as_elf == nullptr) {
				success &= masm::elf::pack_image(proj, std::get<driver_t::image_t>(value));
				result_work.emplace_back(driver_t::work_iterable_t::value_type{stage_t::PACK, proj->as_elf});
			}
		}
		return driver_t::result_t{success, result_work};
	};
	driver->register_transform(tx_elf, stage_t::ADDRESS_ASSIGN);

	transform_t tx_end = [=](project_t& proj, std::list<driver_t::work_t>& work) {
		driver_t::work_iterable_t result_work;
		std::transform(work.begin(), work.end(), std::back_inserter(result_work),[&](auto elf_image){
			return driver_t::work_iterable_t::value_type{stage_t::FINISHED, elf_image};
		});
		return driver_t::result_t{true, result_work};
	};
	driver->register_transform(tx_end, stage_t::PACK);

	return driver;
}

std::tuple<bool, std::shared_ptr<masm::project::project<uint16_t>>> asmb::pep10::driver::assemble(std::string user_text, std::string os_text)
{
	using namespace asmb::pep10::driver;
	auto driver = make_driver();
	auto project = masm::project::init_project<uint16_t>();
	auto ex = registry::instance();
	for(const auto& macro : ex.macros()) project->macro_registry->register_macro(macro.name, macro.text, masm::MacroType::CoreMacro);
	auto os = std::make_shared<masm::project::source_file>();
	os->name = "os";
	os->body = os_text;
	auto user = std::make_shared<masm::project::source_file>();
	user->name = "user";
	user->body = user_text;
	auto res = driver->assemble_joint(project, os, user, masm::project::toolchain_stage::PACK);
	return {res.first, project};
}
#include "create_driver.hpp"

#include "masm/backend/sanity.hpp"
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
		std::map<driver_t::section_t, bool> parsed_tls;
		std::transform(work.begin(), work.end(), std::back_inserter(result_work),[&](auto& value){
				auto containing_image = std::get<driver_t::section_t>(value)->containing_image.lock();
				std::shared_ptr<masm::elf::code_section<uint16_t>> tls = containing_image->section;
				if(parsed_tls.find(tls) != parsed_tls.end()) {
					bool val = parsed_tls[tls] = whole_program_sanity_fixup<uint16_t>(proj, tls);
					success &= val;
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
			masm::backend::assign_image(proj, image);
			return driver_t::work_iterable_t::value_type{stage_t::ADDRESS_ASSIGN, image};
		});
		return driver_t::result_t{success, result_work};
	};
	driver->register_transform(tx_addr, stage_t::WHOLE_PROGRAM_SANITY);

	transform_t tx_elf = [=](project_t& proj, std::list<driver_t::work_t>& work) {
		bool success = true;
		driver_t::work_iterable_t result_work;
		std::transform(work.begin(), work.end(), std::back_inserter(result_work),[&](auto value){
			auto [local_success, stream] = masm::elf::pack_image(proj, std::get<driver_t::image_t>(value));
			success &= local_success;
			return driver_t::work_iterable_t::value_type{stage_t::PACK, value};
		});
		return driver_t::result_t{success, result_work};
	};
	driver->register_transform(tx_elf, stage_t::ADDRESS_ASSIGN);
	return driver;
}
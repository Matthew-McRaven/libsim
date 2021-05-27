#include "driver.hpp"

#include <algorithm>

#include "masm/frontend/imagize.hpp"

template <typename address_size_t, typename stage_t>
std::pair<bool, std::string> masm::driver<address_size_t, stage_t>::assemble_os(
	project_t& project, std::shared_ptr<masm::project::source_file> os, stage_t target_stage)
{
	masm::frontend::text_to_image(project, os, nullptr);
	
	work_queue_[stage_t::RAW] = {};
	for(auto &[index, image] : project->images) {
		work_t val = {std::static_pointer_cast<masm::elf::code_section<address_size_t>>(image->os)};
		work_queue_[stage_t::RAW].emplace_back(val);
	}

	bool failed_stage=false;
	// Process all sections!
	while(!work_queue_.empty()) {

		// Begin work on section with the lowest current stage.
		auto [stage, work] = *work_queue_.begin();
		work_queue_.erase(stage);

		// Ignore stages beyond our target_stage
		if(stage >= target_stage) continue;

		// Find a transformation if it exists. If it doesn't exist, then we can go no further than this stage.
		if(auto transform = transforms_.find(stage); 
		   transform == transforms_.end())  {
			// If one stage failed to find the transform, every stage will fail. Just give up now.
			return {false, fmt::format(";ERROR: No transform for stage {}.", target_stage)};
		}
		else {
			auto [stage_success, new_work] = transform->second(project, work);
			if(!stage_success) {
				failed_stage = true;
				// If we failed a stage before our target (e.g. target,stage:=LINKER,PREPROCESS), force all sections
				// to stop at the current stage, because we can't fix errors in our assembler.
				target_stage = std::min(target_stage, stage);
			}
			else {
				for(auto item : new_work) {
					if(work_queue_.find(item.first) == work_queue_.end()) work_queue_[item.first] = {};
					work_queue_[item.first].emplace_back(item.second);
				}
			}
		}
		
	}
	if(failed_stage) return {false, fmt::format(";ERROR: Transform failed for stage {}.", target_stage)};
	return {true, ""};
}

template <typename address_size_t, typename stage_t>
std::pair<bool, std::string> masm::driver<address_size_t, stage_t>::assemble_joint(
	project_t& project, std::shared_ptr<masm::project::source_file> os, 
	std::shared_ptr<masm::project::source_file> user, stage_t target_stage)
{
	masm::frontend::text_to_image(project, os, user);
	
	work_queue_[stage_t::RAW] = {};
	for(auto &[index, image] : project->images) {
		work_t val = {std::static_pointer_cast<masm::elf::code_section<address_size_t>>(image->os)};
		work_queue_[stage_t::RAW].emplace_back(val);
		if(user){
			val = {std::static_pointer_cast<masm::elf::code_section<address_size_t>>(image->user)};
			work_queue_[stage_t::RAW].emplace_back(val);
		}
	}

	bool failed_stage=false;
	// Process all sections!
	while(!work_queue_.empty()) {

		// Begin work on section with the lowest current stage.
		auto [stage, work] = *work_queue_.begin();
		work_queue_.erase(stage);

		// Ignore stages beyond our target_stage
		if(stage >= target_stage) continue;

		// Find a transformation if it exists. If it doesn't exist, then we can go no further than this stage.
		if(auto transform = transforms_.find(stage); 
		   transform == transforms_.end())  {
			// If one stage failed to find the transform, every stage will fail. Just give up now.
			return {false, fmt::format(";ERROR: No transform for stage {}.", target_stage)};
		}
		else {
			auto [stage_success, new_work] = transform->second(project, work);
			if(!stage_success) {
				failed_stage = true;
				// If we failed a stage before our target (e.g. target,stage:=LINKER,PREPROCESS), force all sections
				// to stop at the current stage, because we can't fix errors in our assembler.
				target_stage = std::min(target_stage, stage);
			}
			else {
				for(auto item : new_work) {
					if(work_queue_.find(item.first) == work_queue_.end()) work_queue_[item.first] = {};
					work_queue_[item.first].emplace_back(item.second);
				}
			}
		}
		
	}
	if(failed_stage) return {false, fmt::format(";ERROR: Transform failed for stage {}.", target_stage)};
	return {true, ""};
}

template <typename address_size_t, typename stage_t>
bool masm::driver<address_size_t, stage_t>::register_transform(
	typename masm::driver<address_size_t, stage_t>::transform_t transform, stage_t stage)
{
	if(transforms_.find(stage) != transforms_.end()) return false;
	transforms_[stage] = transform;
	return true;
}
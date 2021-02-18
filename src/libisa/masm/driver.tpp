#include "driver.hpp"

template <typename address_size_t, typename stage_t>
std::pair<bool, std::string> masm::driver<address_size_t, stage_t>::assemble_project(
	std::shared_ptr<masm::project::project<address_size_t> >& project, 
	std::vector<masm::project::source_file> sources, stage_t target_stage)
{
	bool okay = masm::frontend::section_program<address_size_t>(project, sources, ".TEXT");
	if(!okay) return {false, ";ERROR: Failed to section program"};

	bool missing_stage_transform=false, failed_stage=false;
	// Process all sections!
	while(!project->section_queue.empty()) {

		// Begin work on section with the lowest current stage.
		auto [stage, section] =  project->section_queue.top();
		project->section_queue.pop(); 

		// Ignore stages beyond our target_stage
		if(stage >= target_stage) continue;

		// Find a transformation if it exists. If it doesn't exist, then we can go no further than this stage.
		if(auto transform = transforms_.find(stage); transform == transforms_.end())  {
			// If one stage failed to find the transform, every stage will fail. Just give up now.
			return {false, fmt::format(";ERROR: No transform for stage {}.", target_stage)};;
		}
		else {
			auto [new_stage, stage_success] = transform->second(project, section);
			if(!stage_success) {
				failed_stage = true;
				// If we failed a stage before our target (e.g. target,stage:=LINKER,PREPROCESS), force all sections
				// to stop at the current stage, because we can't fix errors in our assembler.
				target_stage = std::min(target_stage, stage);
			}
			else {
				project->section_queue.push({new_stage, section});
			}
		}
		
	}
	if(missing_stage_transform) 
	else if(failed_stage) return {false, fmt::format(";ERROR: Transform failed for stage {}.", target_stage)};
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
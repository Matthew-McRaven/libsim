#include "masm/frontend/sectionize.hpp"
#include "masm/project/project.hpp"

template <typename address_size_t>
bool masm::frontend::section_program(std::shared_ptr<masm::project::project<address_size_t>>& project, 
	std::vector<masm::project::source_file> sources, const std::string& default_section
) {
	bool okay = true;
	int image_index = 0;

	// Convert all source files to images by feeding them through our sectionizer
	for(const auto& source : sources) {
		auto prog = sectionize(source.body, default_section);
		auto prog_image = std::make_shared<masm::elf::image<address_size_t>>();
		// Assign each section an incrementally increased section id.
		int source_section=0;
		std::transform(prog.begin(), prog.end(), std::back_inserter(prog_image->sections), 
		// Convert from ::section_text to masm::elf::code_section<T>.
		[&source_section](const auto text) { 
			auto x = std::make_shared<masm::elf::top_level_section<address_size_t>>();
			x->header.name = text.section_name;
			x->header.index = source_section++;
			x->body_raw = masm::elf::code::raw();
			x->body_raw.value().text = boost::algorithm::join(text.lines, "");
			x->body_raw.value().lines = text.lines;
			return x; 
			}
		);
		// Mantain project invariant that every section is registered in priority queue.
		for(auto section : prog_image->sections) {
			project->section_queue.push(std::make_pair(masm::project::toolchain_stage::RAW, section));
		}
		project->images[image_index++] = prog_image;
		
	}

	return okay;
}
#include "masm/frontend/sectionize.hpp"
#include "masm/project/project.hpp"

template <typename address_size_t>
std::vector<std::shared_ptr<masm::elf::top_level_section<address_size_t> > >
masm::frontend::section_program(std::shared_ptr<masm::project::project<address_size_t>>& project, 
	std::vector<std::shared_ptr<masm::project::source_file>> sources, const std::string& default_section
) {
	int image_index = 0;
	auto sections = std::vector<std::shared_ptr<masm::elf::top_level_section<address_size_t> > >{};

	// Convert all source files to images by feeding them through our sectionizer
	for(const auto& source : sources) {
		auto prog = sectionize(source->body, default_section);
		auto prog_image = std::make_shared<masm::elf::image<address_size_t>>();
		// Assign each section an incrementally increased section id.
		int source_section=0;
		std::transform(prog.begin(), prog.end(), std::back_inserter(prog_image->sections), 
		// Convert from ::section_text to masm::elf::code_section<T>.
		[&source_section](const auto& text) { 
			auto x = std::make_shared<masm::elf::top_level_section<address_size_t>>();
			x->header.name = text.section_name;
			x->header.index = source_section++;
			x->body_raw = masm::elf::code::raw();
			x->body_raw.value().text = boost::algorithm::join(text.lines, "");
			x->body_raw.value().lines = text.lines;
			return x; 
			}
		);
		std::copy(prog_image->sections.begin(), prog_image->sections.end(), std::back_inserter(sections)); 
		project->images[image_index++] = prog_image;
		
	}

	return sections;
}
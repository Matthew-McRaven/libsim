#pragma once

#include <string>

#include "masm/project/project.hpp"
#include "masm/project/image.hpp"

// Needed to join on a delimter
#include <boost/algorithm/string/join.hpp>

namespace masm::frontend
{
	// A single result from sectionize(...).
	// Contains a group of source lines (as a vector) that correspond to a single assembly level section.
	struct section_text
	{
		size_t document_line; // What line in the document begins this section?
		std::string section_name;
		std::vector<std::string> lines;
	};
	// Split a program based on the sections it contains.
	std::vector<section_text> sectionize(const std::string& program, const std::string& default_section);


	template <typename address_size_t>
	bool section_program(std::shared_ptr<masm::project::project<address_size_t>>& project, 
		std::vector<masm::project::source_file> sources, const std::string& default_section=".text"
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
				x->body_raw.value().text = boost::algorithm::join(text.lines, "\n");
				x->body_raw.value().lines = text.lines;
				return x; 
				}
			);
			project->images[image_index++] = prog_image;
		}

		return okay;
	}
} // End namespace masm::frontend
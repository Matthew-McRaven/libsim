#include "imagize.hpp"

#include "masm/project/image.hpp"
#include <string>
#include <vector>

template <typename address_size_t>
std::shared_ptr<masm::elf::image<address_size_t>> masm::frontend::text_to_image(
	std::shared_ptr<masm::project::project<address_size_t>>& project, 
	std::shared_ptr<masm::project::source_file> os
) {
	auto image = std::make_shared<masm::elf::image<address_size_t> >();
	image->symbol_table = std::make_shared<symbol::BranchTable<address_size_t>>();
	auto section = std::make_shared<masm::elf::top_level_section<address_size_t>>();
	section->symbol_table = symbol::insert_leaf<address_size_t>({image->symbol_table});
	section->header.name = "os";
	section->header.index = 0;
	section->containing_image = image;
	section->body_raw = masm::elf::code::raw();
	section->body_raw.value().text = os->body;
	std::vector<std::string> as_lines;
	// Courtesy of:
	// 	https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
	boost::split(as_lines, os->body, boost::is_any_of("\n"));
	// Prevent an erronious extra (blank) line from being added to our document.
	if(as_lines.back() == "") {
		as_lines.pop_back();
	}
	// Re-added \n to make tokenizer happier.s
	for(auto& line : as_lines) line.append("\n");
	section->body_raw.value().lines = as_lines;
	image->section = section;
	project->images[0] = image;
	return image;
}
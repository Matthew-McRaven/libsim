#include "imagize.hpp"

#include "masm/project/image.hpp"
#include <string>
#include <vector>

template<typename address_size_t>
std::shared_ptr<masm::elf::top_level_section<address_size_t>> text_to_image_helper(
	std::shared_ptr<masm::elf::image<address_size_t>> image, std::shared_ptr<masm::project::source_file> prog, 
	int header_index, std::string section_name)
{
	// Construct a section from
	auto section = std::make_shared<masm::elf::top_level_section<address_size_t>>();
	section->symbol_table = symbol::insert_leaf<address_size_t>({image->symbol_table});
	section->header.name = section_name;
	section->header.index = header_index;
	section->containing_image = image;
	section->body_raw = masm::elf::code::raw();
	section->body_raw.value().text = prog->body;
	std::vector<std::string> as_lines;
	// Courtesy of:
	// 	https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
	boost::split(as_lines, prog->body, boost::is_any_of("\n"));
	// Prevent an erronious extra (blank) line from being added to our document.
	if(as_lines.back() == "") {
		as_lines.pop_back();
	}
	// Re-added \n to make tokenizer happier.s
	for(auto& line : as_lines) line.append("\n");
	section->body_raw.value().lines = as_lines;

	return section;
}
template <typename address_size_t>
std::shared_ptr<masm::elf::image<address_size_t>> masm::frontend::text_to_image(
	std::shared_ptr<masm::project::project<address_size_t>>& project, 
	std::shared_ptr<masm::project::source_file> os,
	std::shared_ptr<masm::project::source_file> user
) {
	// Construct image to hold both OS and user program.
	auto image = std::make_shared<masm::elf::image<address_size_t>>();
	image->symbol_table = std::make_shared<symbol::BranchTable<address_size_t>>();

	// We must have an OS.
	assert(os);
	auto sec_os = text_to_image_helper(image, os, 0, "os.text");
	sec_os->header.section_type = masm::elf::program_type::kOperatingSystem;
	// But not necessarily a user program.
	std::shared_ptr<masm::elf::top_level_section<address_size_t>> sec_user = nullptr;
	if(user) {
		sec_user = text_to_image_helper(image, user, 1, "user.text");
		sec_user->header.section_type = masm::elf::program_type::kUserProgram;
	}
	image->os = sec_os;
	image->user = sec_user;
	project->images[project->images.size()] = image;
	return image;
}

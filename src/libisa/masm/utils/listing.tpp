#include "listing.hpp"

#include <fmt/core.h>

#include "utils/format.hpp"
template <typename addr_size_t>
std::string masm::utils::generate_listing(std::shared_ptr<masm::elf::top_level_section<addr_size_t> >& image)
{
	std::stringstream out_stream;
	for(const auto& line : image->body_ir->ir_lines){
		out_stream << line->generate_listing_string() << std::endl;
	}
	return out_stream.str();
}

template <typename addr_size_t>
std::vector<uint8_t> masm::utils::get_bytecode(std::shared_ptr<masm::elf::top_level_section<addr_size_t> >& image)
{
	std::vector<uint8_t> object_code;
	for(const auto& line : image->body_ir->ir_lines) line->append_object_code(object_code);
	return object_code;
}

template <typename addr_size_t>
std::string masm::utils::generate_formatted_bytecode(std::shared_ptr<masm::elf::top_level_section<addr_size_t> >& image,
	uint8_t bytes_per_line)
{
	assert(bytes_per_line > 0);

	auto object_code =  get_bytecode(image);
	return ::utils::bytes_to_hex_string(object_code, bytes_per_line, true);

}


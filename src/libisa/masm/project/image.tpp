#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <cereal/archives/portable_binary.hpp>

#include "elf_tools/elf_helper.hpp"

template <typename address_size_t>
masm::elf::AnnotatedImage<address_size_t>::AnnotatedImage(std::shared_ptr<ELFIO::elfio> image):
	_ptr(image)
{
	using namespace boost::iostreams;
	static const auto init_ptr = [](auto& line_obj, auto& line_bytes){
		if(line_bytes.has_value()) {
			line_obj = std::make_shared<masm::elf::AddressLineRegionVector<address_size_t>>();
			// Bytes to istream. See: https://stackoverflow.com/a/12646922
			auto byte_vec = line_bytes.value();
			basic_array_source<char> input_source((char*)byte_vec.data(), byte_vec.size());
			stream<basic_array_source<char>> input_stream(input_source);
			cereal::PortableBinaryInputArchive archive(input_stream);
			archive(*line_obj);
		}
	};
	
	auto user_lines_bytes = elf_tools::section_as_bytes(*_ptr, "user.lines");
	auto os_lines_bytes = elf_tools::section_as_bytes(*_ptr, "os.lines");
	init_ptr(_user_lines, user_lines_bytes);
	init_ptr(_os_lines, os_lines_bytes);

}

template <typename address_size_t>
std::optional<std::size_t> masm::elf::AnnotatedImage<address_size_t>::listing_line_from_address(address_size_t address) const 
{
	if(_os_lines && _os_lines->contains(address)) return _os_lines->line(address);
	if(_user_lines && _user_lines->contains(address)) return _user_lines->line(address);
	return std::nullopt;
}

template <typename address_size_t>
ELFIO::elfio& masm::elf::AnnotatedImage<address_size_t>::image()
{
	return *_ptr;
}
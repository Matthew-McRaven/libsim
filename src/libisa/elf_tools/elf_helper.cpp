#include "elf_tools/elf_helper.hpp"


ELFIO::section* elf_tools::find_section(ELFIO::elfio& image, const std::string& name)
{
	ELFIO::section* sec = nullptr;
	// Section 0 contains nothing useful, so we can skip it with preincrement.
	for(auto i=0; i<image.sections.size();++i) {
		auto local_sec = image.sections[i];
		if(local_sec->get_name() == name) sec = local_sec;
	}
	return sec;
}

ELFIO::section* elf_tools::find_section(const ELFIO::elfio& image, const std::string& name)
{
	ELFIO::section* sec = nullptr;
	// Section 0 contains nothing useful, so we can skip it with preincrement.
	for(auto i=0; i<image.sections.size();++i) {
		auto local_sec = image.sections[i];
		if(local_sec->get_name() == name) sec = local_sec;
	}
	return sec;
}

result<std::vector<uint8_t>> elf_tools::section_as_bytes(const ELFIO::elfio& image, const std::string& name)
{
	auto sec = find_section(image, name);
	if(sec == nullptr) return {system_error2::errc::invalid_argument};

	return std::vector<uint8_t>(sec->get_data(), sec->get_data()+sec->get_size());
}
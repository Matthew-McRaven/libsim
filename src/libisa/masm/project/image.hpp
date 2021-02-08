#pragma once
#include "section.hpp"
namespace masm::elf {
	template <typename address_size_t>
	struct image
	{
		address_size_t entry_offset = {0};
		std::shared_ptr<masm::elf::top_level_section<address_size_t> > entry_point = {nullptr};
		std::vector<std::shared_ptr<masm::elf::top_level_section<address_size_t> > > sections;

	};
} // End namespace masm::elf
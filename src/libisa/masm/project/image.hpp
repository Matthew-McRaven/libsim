#pragma once
#include "section.hpp"
namespace masm::elf {
	template <typename address_size_t>
	struct image
	{
		std::shared_ptr<symbol::table<address_size_t> > symbol_table = std::make_shared<symbol::table<address_size_t>>();
		std::shared_ptr<masm::elf::top_level_section<address_size_t> > entry_point = {nullptr};
		std::vector<std::shared_ptr<masm::elf::top_level_section<address_size_t> > > sections;

	};
} // End namespace masm::elf
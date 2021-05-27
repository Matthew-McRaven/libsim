#pragma once
#include "section.hpp"
#include "symbol/table.hpp"
namespace masm::elf {
	template <typename address_size_t>
	struct image
	{
		std::shared_ptr<symbol::BranchTable<address_size_t> > symbol_table 
			= std::make_shared<symbol::BranchTable<address_size_t>>();
		std::shared_ptr<masm::elf::top_level_section<address_size_t>> section;

	};
} // End namespace masm::elf
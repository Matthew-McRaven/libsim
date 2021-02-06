#pragma once

#include "masm/project/image.hpp"
namespace masm::elf {
	void dump_dummy_file();

	void dump_pep10_file();

	template <typename address_size_t>
	void image_to_elf_binary(const masm::elf::image<address_size_t>& to_serialize)
	{


		// Create program header.
		// Create strtab.
		// Create symbol table.
		// Create section header str tab.
	};
} // End namespace mams::elf_tools
#pragma once

#include <optional>
#include <string>

#include <elfio/elfio.hpp>

#include "masm/elf/addr_line_mapping.hpp"
#include "outcome_helper.hpp"
#include "section.hpp"
#include "symbol/table.hpp"

namespace masm::elf {
	template <typename address_size_t>
	struct image
	{
		std::shared_ptr<symbol::BranchTable<address_size_t> > symbol_table 
			= std::make_shared<symbol::BranchTable<address_size_t>>();
		std::shared_ptr<masm::elf::top_level_section<address_size_t>> os, user;

	};

	template <typename address_size_t>
	class AnnotatedImage
	{
	public:
		AnnotatedImage(std::shared_ptr<ELFIO::elfio> image);
		std::optional<std::size_t> listing_line_from_address(address_size_t address) const;
		std::optional<address_size_t> address_from_listing_line(std::size_t address) const;
		ELFIO::elfio& image();
	private:
		std::shared_ptr<masm::elf::AddressLineRegionVector<address_size_t>> _user_lines{nullptr}, _os_lines{nullptr};
		std::shared_ptr<ELFIO::elfio> _ptr;
	};
} // End namespace masm::elf

#include "image.tpp"
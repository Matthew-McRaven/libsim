#pragma once
#include <iostream>

#include <elfio/elfio.hpp>
/*
 * At this point in the project, I needed some humor.
 * The methods and classes described in this file are responsible for turning our IR into an ELF binary.
 * That is, this code creates elves.
 * 
 * According to LoTR mythology, the supreme being Eru Ilúvatar creates elf-kind from his thoughts.
 * Seeing as both this code and Eru are in the business of making elves, it seemed fitting to name the code after Eru.
 * See: https://lotr.fandom.com/wiki/Eru_Il%C3%BAvatar
 */

namespace masm::elf
{

	template <typename addr_size_t>	
	void addr_line_mapping(std::ostream& os, const masm::elf::top_level_section<addr_size_t>& section);
		
	template <typename addr_size_t>
	bool pack_image(std::shared_ptr<masm::project::project<addr_size_t> >& project, 
		std::shared_ptr<masm::elf::image<addr_size_t> >& image);

	template <typename addr_size_t>
	auto eru = pack_image<addr_size_t>;
}
#include "pack.tpp"
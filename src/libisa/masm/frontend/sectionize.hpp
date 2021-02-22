#pragma once

#include <string>

#include "masm/project/project.hpp"
#include "masm/project/image.hpp"

// Needed to join on a delimter
#include <boost/algorithm/string/join.hpp>

namespace masm::frontend
{

// A single result from sectionize(...).
// Contains a group of source lines (as a vector) that correspond to a single assembly level section.
struct section_text
{
	std::size_t document_line; // What line in the document begins this section?
	std::string section_name;
	std::vector<std::string> lines;
};
// Split a program based on the sections it contains.
std::vector<section_text> sectionize(const std::string& program, const std::string& default_section);


template <typename address_size_t>
std::vector<std::shared_ptr<masm::elf::top_level_section<address_size_t> > > section_program(
	std::shared_ptr<masm::project::project<address_size_t>>& project, 
	std::vector<std::shared_ptr<masm::project::source_file>> sources, const std::string& default_section=".text"
);

} // End namespace masm::frontend

#include "masm/frontend/sectionize.tpp"
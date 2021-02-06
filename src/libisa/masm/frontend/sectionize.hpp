#pragma once

#include <string>

#include "masm/project/project.hpp"

namespace masm::frontend
{
	template <typename address_size_t>
	bool decompose_into_sections(std::shared_ptr<masm::project::project<address_size_t>>& project, const std::string& program)
	{

	}

	struct section_text
	{
		std::string section_name;
		std::vector<std::string> lines;
	};
	std::list<section_text> sectionize(std::string program, std::string default_section=".text");
} // End namespace masm::fronted
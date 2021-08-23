#pragma once

#include <string>

#include <elfio/elfio.hpp>

namespace elf_tools{

// Return a pointer to the section with the given name, or nullptr if not found.
ELFIO::section* find_section(ELFIO::elfio& image, const std::string& name);
const ELFIO::section* find_section(const ELFIO::elfio& image, const std::string& name);

}; // End namespace elf_tools
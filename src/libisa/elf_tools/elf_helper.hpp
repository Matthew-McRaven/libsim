#pragma once

#include <string>

#include <elfio/elfio.hpp>

namespace elf_tools{

// Return a pointer to the section with the given name, or nullptr if not found.
// Unlike the other overload, feel free to modify the section through the returned pointer.
ELFIO::section* find_section(ELFIO::elfio& image, const std::string& name);

// This is a little wonky: A const object will give you a non-const pointer to something it contains.
// However, ELFIO accessors only work on mutable pointers, not const pointer.
// I beg you, if you call this const onverload, do not modify the object through the returned pointer.
ELFIO::section* find_section(const ELFIO::elfio& image, const std::string& name);

}; // End namespace elf_tools
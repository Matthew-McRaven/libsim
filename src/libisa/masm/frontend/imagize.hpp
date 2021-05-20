#pragma once

#include "masm/project/project.hpp"
#include "masm/project/image.hpp"

namespace masm::frontend{
template <typename address_size_t>
std::shared_ptr<masm::elf::image<address_size_t>> text_to_image(
	std::shared_ptr<masm::project::project<address_size_t>>& project, 
	std::shared_ptr<masm::project::source_file> source
);
}
#include "imagize.tpp"
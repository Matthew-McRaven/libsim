#pragma once 

#include <climits>
#include <functional>
#include <map>
#include <queue>
#include <tuple>
#include <variant>
#include <vector>

#include "masm/project/image.hpp"
#include "masm/project/message_handler.hpp"
#include "masm/registry.hpp"
#include "symbol/table.hpp"
namespace masm::project {

enum class toolchain_stage
{
	RAW,
	TOKEN,
	PREPROCESS,
	SYNTAX,
	SYMANTIC,
	WHOLE_PROGRAM_SANITY, // Sanity checks like "does the image end in .BURN?"
	ADDRESS_ASSIGN,
	DEBUG0,
	DEBUG1,
	PACK,
	INTERLINK,
	SANITY,
	FINISHED
};

// Represents a single input to our assembler toolchain.
struct source_file
{
	std::string name;
	std::string body;
};

template <typename address_size_t>
struct project
{
	std::shared_ptr<masm::macro_registry> macro_registry;
	std::shared_ptr<message_handler<address_size_t> > message_resolver;
	std::shared_ptr<masm::elf::image<address_size_t>> image;
	std::shared_ptr<masm::elf::AnnotatedImage<address_size_t>> as_elf;
};

}
#include "project.tpp"
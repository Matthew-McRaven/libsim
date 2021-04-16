#pragma once 

#include <climits>
#include <map>
#include <tuple>
#include <variant>
#include <vector>
#include <queue>
#include <functional> 

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
	std::map<uint32_t, std::shared_ptr<masm::elf::image<address_size_t> > > images;
};

}
#include "project.tpp"
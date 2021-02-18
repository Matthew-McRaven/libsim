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
	INTRALINK,
	DEBUG0,
	DEBUG1,
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
	std::shared_ptr<symbol::SymbolTable<address_size_t> > symbol_table;
	std::shared_ptr<message_handler<address_size_t> > message_resolver;
	std::map<uint32_t, std::shared_ptr<masm::elf::image<address_size_t> > > images;
	// All sections in all images must be registered in the section queue.
	// TODO: Wrap in accessors, because now we have invariants to mantain.
	using queue_state_t =  std::pair<toolchain_stage, std::shared_ptr<masm::elf::code_section<address_size_t> > >;
	std::priority_queue <queue_state_t, std::vector<queue_state_t>, std::greater<queue_state_t> > section_queue;
};

}
#include "project.tpp"
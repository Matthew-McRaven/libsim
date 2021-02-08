#pragma once 

#include <climits>
#include <map>
#include <tuple>
#include <variant>
#include <vector>

#include "masm/project/image.hpp"
#include "masm/registry.hpp"
#include "symbol/table.hpp"
namespace masm::project {
template <typename address_size_t>
struct project
{
	std::shared_ptr<masm::macro_registry> macro_registry;
	std::shared_ptr<symbol::SymbolTable<address_size_t>> symbol_table;
	void* message_handler;
	std::map<uint32_t, masm::elf::image<address_size_t> > images;
};

}
#include "project.tpp"
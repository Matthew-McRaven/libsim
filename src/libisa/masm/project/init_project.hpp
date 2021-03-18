#pragma once
#include <string>

#include "masm/project/project.hpp"
#include "masm/registry.hpp"
#include "symbol/table.hpp"
namespace masm::project
{
	// Utility function to create an initialized project for a user program.
	template <typename address_size_t>
	auto init_project() -> std::shared_ptr<masm::project::project<address_size_t> >
	{
		auto x = std::make_shared<masm::project::project<address_size_t> >();
		x->macro_registry = std::make_shared<masm::macro_registry>();
		x->message_resolver = std::make_shared<masm::message_handler<address_size_t> >();
		return x;
	}

} // End namespace masm::frontend
#pragma once
#include "asmb/pep10/ir.hpp"
#include "masm/ir/directives.hpp"
#include "masm/ir/args.hpp"
#include "masm/ir/macro.hpp"
#include "symbol/value.hpp"
#include "masm/project/section.hpp"
#include "masm/project/project.hpp"
//#include "masm/ir.macro.hpp"

template<typename address_size_t>
bool whole_program_sanity_fixup(std::shared_ptr<masm::project::project<uint16_t> >& project, 
	std::shared_ptr<masm::elf::code_section<uint16_t> >& section);

#include "sanity.tpp"
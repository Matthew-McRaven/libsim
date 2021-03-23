#pragma once


#include "masm/frontend/tokens.hpp"
#include "masm/frontend/tokenizer.hpp"
#include "asmb/pep10/tokenizer.hpp"
#include "asmb/pep10/parser.hpp"
#include "asmb/pep10/ir.hpp"
#include "masm/registry.hpp"
#include "masm/project/project.hpp"
#include "masm/project/init_project.hpp"
#include "masm/driver.hpp"
#include "masm/ir/directives.hpp"
#include "masm/backend/assign_addr.hpp"



namespace asmb::pep10::assign_addr{
	using tokenizer_t = asmb::pep10::tokenizer<masm::frontend::lexer_t>;
	using stage_t = masm::project::toolchain_stage;
	using driver_t = masm::driver<uint16_t, stage_t>;
	using project_t = driver_t::project_t;
	using section_t = driver_t::section_t;
	using result_t = driver_t::result_t;
	using transform_t = driver_t::transform_t;
	std::shared_ptr<driver_t> make_driver();
};
#include "catch.hpp"

#include "isa/pep10/instruction.hpp"
#include "masm/registry.hpp"
#include "masm/frontend/preprocesser.hpp"
#include "masm/project/section.hpp"
#include "masm/frontend/sectionize.hpp"

TEST_CASE( "Preprocess" ) {
	/*
	SECTION("Implicit section") {
		auto project = masm::frontend::init_user_project<uint16_t>();
		masm::project::source_file file;
		file.name = "main";
		file.body = "LWDA 20,d\n.END\n";
		CHECK(masm::frontend::section_program<uint16_t>(project, {file}));
		auto preproc = masm::frontend::preprocessor<uint16_t>();
		preproc.preprocess(project, decltype(preproc)::flags());
	}

	SECTION("0-arity macro") {
		auto project = masm::frontend::init_user_project<uint16_t>();
		CHECK(project->macro_registry->register_macro("HELLO", "@HELLO 0\n.END\n", masm::MacroType::CoreMacro));
		masm::project::source_file file;
		file.name = "main";
		file.body = "@HELLO\n.END\n";
		CHECK(masm::frontend::section_program<uint16_t>(project, {file}));
		auto preproc = masm::frontend::preprocessor<uint16_t>();
		preproc.preprocess(project, decltype(preproc)::flags());
	}

	SECTION("0-arity macro, with comment") {
		auto project = masm::frontend::init_user_project<uint16_t>();
		CHECK(project->macro_registry->register_macro("HELLO", "@HELLO 0\n.END\n", masm::MacroType::CoreMacro));
		masm::project::source_file file;
		file.name = "main";
		file.body = "@HELLO;dingus\n.END\n";
		CHECK(masm::frontend::section_program<uint16_t>(project, {file}));
		auto preproc = masm::frontend::preprocessor<uint16_t>();
		preproc.preprocess(project, decltype(preproc)::flags());
	}
	*/
	// TODO: Bad section definitions.
	// TODO: Check "empty" default section.
}
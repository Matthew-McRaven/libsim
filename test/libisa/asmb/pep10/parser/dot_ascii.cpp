#include "catch.hpp"

#include "asmb/pep10/ir.hpp"
#include "asmb/pep10/create_driver.hpp"
#include "masm/ir/directives.hpp"

TEST_CASE( "Parse dot ascii", "[asmb::pep10::parser]"  ) {
	using namespace asmb::pep10::driver;
	auto driver = make_driver();

	SECTION("Valid .ASCII") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s:asra\n.ASCII \"Hello\"\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->os;
		REQUIRE(project->images[0]->os->body_ir->ir_lines.size() == 2);
		auto maybe_ascii = project->images[0]->os->body_ir->ir_lines[1];
		auto as_ascii = std::dynamic_pointer_cast<masm::ir::dot_ascii<uint16_t> >(maybe_ascii);
		REQUIRE(as_ascii);
	}

	SECTION("Valid .ASCII + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".ASCII \"s\" ;Hi guys\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->os;
		REQUIRE(project->images[0]->os->body_ir->ir_lines.size() == 1);
		auto maybe_ascii = project->images[0]->os->body_ir->ir_lines[0];
		auto as_ascii = std::dynamic_pointer_cast<masm::ir::dot_ascii<uint16_t> >(maybe_ascii);
		REQUIRE(as_ascii);
		REQUIRE(as_ascii->comment);
		CHECK(as_ascii->comment.value() == "Hi guys");
	}

	SECTION("Valid .ASCII + symbol + comment") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "s: .ASCII \"sb\" ;Hi guys\n"; // Self reference is actually okay here, but has no use.
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE(res.first);
		auto x = project->images[0]->os;
		REQUIRE(project->images[0]->os->body_ir->ir_lines.size() == 1);
		auto maybe_ascii = project->images[0]->os->body_ir->ir_lines[0];
		auto as_ascii = std::dynamic_pointer_cast<masm::ir::dot_ascii<uint16_t> >(maybe_ascii);
		REQUIRE(as_ascii);
		REQUIRE(as_ascii->comment);
		REQUIRE(as_ascii->symbol_entry);
		CHECK(as_ascii->symbol_entry->name == "s");
	}

	SECTION("No dec in .ASCII") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".ASCII 22\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}

	SECTION("No hex in .ASCII") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".ASCII 0xbeef\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No signed dec in .ASCII") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".ASCII -19\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No identifer in .ASCII") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".ASCII HI\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
	SECTION("No char in .ASCII") {	
		auto project = masm::project::init_project<uint16_t>();
		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = ".ASCII 'HI'\n";
		auto res = driver->assemble_os(project, file, masm::project::toolchain_stage::SYMANTIC);
		REQUIRE_FALSE(res.first);
	}
}
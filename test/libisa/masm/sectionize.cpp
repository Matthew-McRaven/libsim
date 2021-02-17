#include "catch.hpp"

#include "masm/frontend/initproj.hpp"
#include "masm/project/section.hpp"
#include "masm/registry.hpp"
#include "masm/frontend/sectionize.hpp"

TEST_CASE( "Section parser" ) {
	
	SECTION("Implicit section") {
		const auto program = "LWDA 20,d\n.END\n";
		auto image_base = masm::frontend::sectionize(program, "defaults");
		CHECK(image_base[0].lines.size() == 2);
		CHECK(image_base[0].section_name == "defaults");	
	}

	SECTION("Bisectioned") {
		const auto program = "LWDA 20,d\n.SECTION \"data\"\nLDWA 20,d\n.END\n";
		auto image_base = masm::frontend::sectionize(program, "defaults");
		CHECK(image_base.size() == 2);
		CHECK(image_base[0].lines.size() == 1);
		CHECK(image_base[0].section_name == "defaults");
		CHECK(image_base[1].lines.size() == 3);
		CHECK(image_base[1].section_name == "data");
	}
	// TODO: Bad section definitions.
	// TODO: Check "empty" default section.
}

TEST_CASE( "Project Initialization" ) {
	
	SECTION("Implicit section") {
		auto project = masm::frontend::init_user_project<uint16_t>();
		masm::project::source_file file;
		file.name = "main";
		file.body = "LWDA 20,d\n.END\n";
		CHECK(masm::frontend::section_program<uint16_t>(project, {file}));
		CHECK(project->images.size() == 1);
		CHECK(project->images[0]->sections.size() == 1);
		CHECK(project->images[0]->sections[0]->body_raw.value().lines.size() == 2);
	}

	SECTION("Bisectioned") {
		const auto program = "LWDA 20,d\n.SECTION \"data\"\nLDWA 20,d\n.END\n";
		auto image_base = masm::frontend::sectionize(program, "defaults");
		CHECK(image_base.size() == 2);
		CHECK(image_base[0].lines.size() == 1);
		CHECK(image_base[0].section_name == "defaults");
		CHECK(image_base[1].lines.size() == 3);
		CHECK(image_base[1].section_name == "data");
	}
	// TODO: Bad section definitions.
	// TODO: Check "empty" default section.
}
#include "catch.hpp"


#include "masm/ir/base.hpp"
#include "masm/ir/args.hpp"
#include "masm/ir/empty.hpp"
#include "masm/ir/directives.hpp"
#include "symbol/table.hpp"
#include "symbol/entry.hpp"
#include "symbol/value.hpp"

TEST_CASE( "Linear IR code lines, 16-bit," ) {
    using namespace Catch::Matchers;
	SECTION("Empty line") {
		auto line = masm::ir::blank_line<uint16_t>();
		CHECK_NOTHROW(line.generate_source_string());
		CHECK_NOTHROW(line.generate_listing_string());
		CHECK(line.object_code_bytes() == 0);
		CHECK_FALSE(line.contains_memory_address());
		CHECK_FALSE(line.tracks_trace_tags());
		CHECK_FALSE(line.is_code());
	}

	SECTION("Comment line") {
		std::string comment = "Here is a comment";
		auto line = masm::ir::comment_line<uint16_t>();
		line.comment = comment;
		CHECK(line.generate_source_string().find(comment) != std::string::npos);
		CHECK(line.generate_listing_string().find(comment) != std::string::npos);
		CHECK(line.object_code_bytes() == 0);
		CHECK_FALSE(line.contains_memory_address());
		CHECK_FALSE(line.tracks_trace_tags());
		CHECK_FALSE(line.is_code());
	}

	SECTION("Address line") {
		auto line = masm::ir::dot_address<uint16_t>();
		auto table = symbol::table<uint16_t>();
		auto symbol = table.define("sym");
		auto sym_val = std::make_shared<symbol::value_const<uint16_t>>(0x16);
		symbol->value = sym_val; 
		line.symbol_entry= symbol;
		line.argument = std::make_shared<masm::ir::symbol_ref_argument<uint16_t> >(symbol);
		line.set_begin_address(0x100);

		CHECK(line.generate_source_string() == "sym:     .ADDRSS sym         ");
		CHECK(line.generate_listing_string() == "0x0100       sym:     .ADDRSS sym         ");
		CHECK(line.object_code_bytes() == 2);
		CHECK(line.contains_memory_address());
		CHECK_FALSE(line.tracks_trace_tags());
		CHECK_FALSE(line.is_code());
	}

	SECTION("Align line") {
		auto line = masm::ir::dot_align<uint16_t>();
		line.argument = std::make_shared<masm::ir::dec_argument<uint16_t> >(4);
		line.set_begin_address(0x11);
		line.direction = masm::ir::dot_align<uint16_t>::AlignDirection::kTop;

		CHECK(line.generate_source_string() == "         .ALIGN  4           ");
		CHECK(line.generate_listing_string() == "0x0011 000000         .ALIGN  4           ");
		CHECK(line.object_code_bytes() == 3);
		CHECK(line.contains_memory_address());
		CHECK_FALSE(line.tracks_trace_tags());
		CHECK_FALSE(line.is_code());
	}

	SECTION("ASCII line") {
		auto line = masm::ir::dot_ascii<uint16_t>();
		line.argument = std::make_shared<masm::ir::ascii_argument<uint16_t> >("\"NIT\"", 0xffff);
		line.set_begin_address(0x11);

		CHECK(line.generate_source_string() == "         .ASCII  \"NIT\"       ");
		CHECK(line.generate_listing_string() == "0x0011 4E4954         .ASCII  \"NIT\"       ");
		CHECK(line.object_code_bytes() == 3);
		CHECK(line.contains_memory_address());
		CHECK_FALSE(line.tracks_trace_tags());
		CHECK_FALSE(line.is_code());
	}

	SECTION("Block line") {
		auto line = masm::ir::dot_block<uint16_t>();
		line.argument = std::make_shared<masm::ir::dec_argument<uint16_t> >(3);
		line.set_begin_address(0x11);

		CHECK(line.generate_source_string() == "         .BLOCK  3           ");
		CHECK(line.generate_listing_string() == "0x0011 000000         .BLOCK  3           ");
		CHECK(line.object_code_bytes() == 3);
		CHECK(line.contains_memory_address());
		CHECK(line.tracks_trace_tags());
		CHECK_FALSE(line.is_code());
	}

	SECTION("Burn line") {
		auto line = masm::ir::dot_burn<uint16_t>();
		line.set_begin_address(0x11);
		line.argument = std::make_shared<masm::ir::dec_argument<uint16_t> >(18);

		CHECK(line.generate_source_string() == "         .BURN   18          ");
		CHECK(line.generate_listing_string() == "                      .BURN   18          ");
		CHECK(line.object_code_bytes() == 0);
		CHECK_FALSE(line.contains_memory_address());
		CHECK_FALSE(line.tracks_trace_tags());
		CHECK_FALSE(line.is_code());
	}

	SECTION("Byte line") {
		auto line = masm::ir::dot_byte<uint16_t>();
		line.set_begin_address(0x11);
		line.argument = std::make_shared<masm::ir::dec_argument<uint16_t> >(0xda);

		CHECK(line.generate_source_string() == "         .BYTE   218         ");
		CHECK(line.generate_listing_string() == "0x0011 DA             .BYTE   218         ");
		CHECK(line.object_code_bytes() == 1);
		CHECK(line.contains_memory_address());
		CHECK(line.tracks_trace_tags());
		CHECK_FALSE(line.is_code());
	}

	SECTION("End line") {
		auto line = masm::ir::dot_end<uint16_t>();
		line.set_begin_address(0x11);

		CHECK(line.generate_source_string() == "         .END                ");
		CHECK(line.generate_listing_string() == "                      .END                ");
		CHECK(line.object_code_bytes() == 0);
		CHECK_FALSE(line.contains_memory_address());
		CHECK_FALSE(line.tracks_trace_tags());
		CHECK_FALSE(line.is_code());
	}

	SECTION("Equate line") {
		auto line = masm::ir::dot_equate<uint16_t>();
		line.set_end_address(0x11);
		line.argument = std::make_shared<masm::ir::dec_argument<uint16_t> >(0xca);

		CHECK(line.generate_source_string() == "         .EQUATE 202         ");
		CHECK(line.generate_listing_string() == "                      .EQUATE 202         ");
		CHECK(line.object_code_bytes() == 0);
		CHECK_FALSE(line.contains_memory_address());
		CHECK(line.tracks_trace_tags());
		CHECK_FALSE(line.is_code());
	}

	SECTION("Word line") {
		auto line = masm::ir::dot_word<uint16_t>();
		line.set_begin_address(0x11);
		line.argument = std::make_shared<masm::ir::hex_argument<uint16_t> >(0xcafe);

		CHECK(line.generate_source_string() == "         .BYTE   0xCAFE      ");
		CHECK(line.generate_listing_string() == "0x0011 CAFE           .BYTE   0xCAFE      ");
		CHECK(line.object_code_bytes() == 2);
		CHECK(line.contains_memory_address());
		CHECK(line.tracks_trace_tags());
		CHECK_FALSE(line.is_code());
	}
}
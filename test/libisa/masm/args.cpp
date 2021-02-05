#include "catch.hpp"

#include "masm/ir/args.hpp"

TEST_CASE( "Linear IR arguments, 16-bit, Character Arguments" ) {
    
	SECTION("Basic argument") {
		auto cv = masm::ir::char_argument<uint16_t>("a");
		CHECK(cv.argument_value() == (int)'a');
		CHECK(cv.argument_string() == "a");
	}
	SECTION("Parse hex arguments") {
		auto cv = masm::ir::char_argument<uint16_t>("\\xff");
		CHECK(cv.argument_value() == 255);
		CHECK(cv.argument_string() == "\\xff");

		cv = masm::ir::char_argument<uint16_t>("\\xba");
		CHECK(cv.argument_value() == 0xba);
		CHECK(cv.argument_string() == "\\xba");
	}

	SECTION("Parse escape codes") {
		std::vector<std::pair<std::string, int>> items= {
			{"\\b", (int)'\b'},
			{"\\f", (int)'\f'},
			{"\\n", (int)'\n'},
			{"\\r", (int)'\r'},
			{"\\t", (int)'\t'},
			{"\\v", (int)'\v'},
		};
		for(auto[k, v] : items) {
			auto cv = masm::ir::char_argument<uint16_t>(k);
			CHECK(cv.argument_value() == v);
			CHECK(cv.argument_string() == k);
		}

	}	

	#ifndef EMSCRIPTEN
	SECTION("Disallow multi-character arguments") {
		CHECK_THROWS(masm::ir::char_argument<uint16_t>("ab"));
	}
	SECTION("Bad hex arguments") {
		CHECK_THROWS(masm::ir::char_argument<uint16_t>("\\xf"));
		CHECK_THROWS(masm::ir::char_argument<uint16_t>("\\xfff"));
	}
	#endif
}

TEST_CASE( "Linear IR arguments, 16-bit, Decimal Arguments" ) {
    
	SECTION("Signed positive arguments") {
		auto cv = masm::ir::dec_argument<uint16_t>(255);
		CHECK(cv.argument_value() == 255);
		CHECK(cv.argument_string() == "255");
	}

	SECTION("Signed negative argument") {
		auto cv = masm::ir::dec_argument<uint16_t>(0xffff);
		CHECK(cv.argument_value() == 0xffff);
		CHECK(cv.argument_string() == "-1");
		cv = masm::ir::dec_argument<uint16_t>(0x8000);
		CHECK(cv.argument_value() == 0x8000);
		CHECK(cv.argument_string() == "-32768");
	}

	SECTION("Unsigned positive arguments") {
		auto cv = masm::ir::unsigned_dec_argument<uint16_t>(255);
		CHECK(cv.argument_value() == 255);
		CHECK(cv.argument_string() == "255");
	}

	SECTION("Unsigned negative argument") {
		auto cv = masm::ir::unsigned_dec_argument<uint16_t>(0xffff);
		CHECK(cv.argument_value() == 0xffff);
		CHECK(cv.argument_string() == "65535");
		cv = masm::ir::unsigned_dec_argument<uint16_t>(0x8000);
		CHECK(cv.argument_value() == 0x8000);
		CHECK(cv.argument_string() == "32768");
	}

}

TEST_CASE( "Linear IR arguments, 16-bit, Hexadecimal Arguments" ) {
    
	SECTION("Unsigned positive arguments") {
		auto cv = masm::ir::hex_argument<uint16_t>(255);
		CHECK(cv.argument_value() == 255);
		CHECK(cv.argument_string() == "0x00FF");
	}

	SECTION("Unsigned negative argument") {
		auto cv = masm::ir::hex_argument<uint16_t>(0xffff);
		CHECK(cv.argument_value() == 0xffff);
		CHECK(cv.argument_string() == "0xFFFF");
		cv = masm::ir::hex_argument<uint16_t>(0x8000);
		CHECK(cv.argument_value() == 0x8000);
		CHECK(cv.argument_string() == "0x8000");
	}

}

TEST_CASE( "Linear IR arguments, 16-bit, String Arguments" ) {
    
	SECTION("Basic argument") {
		auto cv = masm::ir::string_argument<uint16_t>("a");
		CHECK(cv.argument_value() == 'a');
		CHECK(cv.argument_string() == "a");
	}
	SECTION("Parse hex arguments") {
		auto cv = masm::ir::string_argument<uint16_t>("\\xff");
		CHECK(cv.argument_value() == 255);
		CHECK(cv.argument_string() == "\\xff");

		cv = masm::ir::string_argument<uint16_t>("\\xba");
		CHECK(cv.argument_value() == 0xba);
		CHECK(cv.argument_string() == "\\xba");
	}

	SECTION("escape codes") {
		std::vector<std::pair<std::string, int>> items= {
			{"\\b", (int)'\b'},
			{"\\f", (int)'\f'},
			{"\\n", (int)'\n'},
			{"\\r", (int)'\r'},
			{"\\t", (int)'\t'},
			{"\\v", (int)'\v'},
			{"\\xde", (int) 0xde},
		};
		for(auto[k, v] : items) {
			auto cv = masm::ir::string_argument<uint16_t>(k);
			CHECK(cv.argument_value() == v);
			CHECK(cv.argument_string() == k);
			// Check that characters can lead.
			cv = masm::ir::string_argument<uint16_t>(k+"b");
			CHECK(cv.argument_value() == (v<<8|'b'));
			CHECK(cv.argument_string() == k+"b");
			// Check that characters can trail.
			cv = masm::ir::string_argument<uint16_t>("b"+k);
			CHECK(cv.argument_value() == ('b'<<8|v));
			CHECK(cv.argument_string() == "b"+k);
			// Check that characters can be in both first and last.
			cv = masm::ir::string_argument<uint16_t>(k+k);
			CHECK(cv.argument_value() == (v<<8|v));
			CHECK(cv.argument_string() == k+k);
		}

	}	

	#ifndef EMSCRIPTEN
	SECTION("Disallow multi-character arguments") {
		CHECK_THROWS(masm::ir::string_argument<uint16_t>("abc"));
	}
	SECTION("Bad hex arguments") {
		CHECK_THROWS(masm::ir::string_argument<uint16_t>("\\xf"));
	}
	#endif
}
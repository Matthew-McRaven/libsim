#include "catch.hpp"


#include "masm/ir/base.hpp"
#include "masm/ir/args.hpp"
#include "masm/ir/empty.hpp"

TEST_CASE( "Linear IR code lines, 16-bit," ) {
    using namespace Catch::Matchers;
	SECTION("Empty line") {
		auto cv = masm::ir::blank_line<uint16_t>();
		CHECK_NOTHROW(cv.generate_source_string());
		CHECK_NOTHROW(cv.generate_listing_string());
	}

	SECTION("Comment line") {
		std::string comment = "Here is a comment";
		auto cv = masm::ir::comment_line<uint16_t>();
		cv.comment = comment;
		CHECK(cv.generate_source_string().find(comment) != std::string::npos);
		CHECK(cv.generate_listing_string().find(comment) != std::string::npos);
	}
}
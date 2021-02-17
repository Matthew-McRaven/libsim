#include "catch.hpp"

#include "masm/frontend/tokens.hpp"
#include "masm/frontend/tokenizer.hpp"
#include "asmb/pep10/tokenizer.hpp"

TEST_CASE( "Hexadecimal Constants", "[masm::tokens]"  ) {
	
	using tokenizer_t = asmb::pep10::tokenizer<masm::frontend::lexer_t>;
	tokenizer_t lexer;
	SECTION("Positive, no leading +") {
		auto result = masm::frontend::tokenize<tokenizer_t>("0x1 0xef 0x333 0xabcd", lexer);
		CHECK(result.success);
		CHECK(result.rest.empty());
		CHECK(!result.error_message);
		CHECK(result.tokens.size() == 4);

		CHECK(result.tokens[0].first == tokenizer_t::token_type::kHexConstant);
		CHECK(result.tokens[0].second == "0x1");

		CHECK(result.tokens[1].first == tokenizer_t::token_type::kHexConstant);
		CHECK(result.tokens[1].second == "0xef");

		CHECK(result.tokens[2].first == tokenizer_t::token_type::kHexConstant);
		CHECK(result.tokens[2].second == "0x333");

		CHECK(result.tokens[3].first == tokenizer_t::token_type::kHexConstant);
		CHECK(result.tokens[3].second == "0xabcd");


	}

	SECTION("Zero") {
		auto result = masm::frontend::tokenize<tokenizer_t>("0x0 0x000", lexer);
		CHECK(result.success);
		CHECK(result.rest.empty());
		CHECK(!result.error_message);
		CHECK(result.tokens.size() == 2);

		CHECK(result.tokens[0].first == tokenizer_t::token_type::kHexConstant);
		CHECK(result.tokens[0].second == "0x0");
				
		CHECK(result.tokens[1].first == tokenizer_t::token_type::kHexConstant);
		CHECK(result.tokens[1].second == "0x000");

	}
	
}
#include "catch.hpp"

#include "masm/frontend/tokens.hpp"
#include "masm/frontend/tokenizer.hpp"
#include "asmb/pep10/tokenizer.hpp"

TEST_CASE( "Identifiers", "[masm::tokens]"  ) {
	
	using tokenizer_t = asmb::pep10::tokenizer<masm::frontend::lexer_t>;
	tokenizer_t lexer;

	SECTION("Alphabetic identifiers") {
		std::vector<std::string> codes = {"Hello" "WoRlD", "ident_ifiers"};
		for(auto entry : codes)
		{
			auto result = masm::frontend::tokenize<tokenizer_t>(fmt::format("   {}   ", entry), lexer);
			CHECK(result.success);
			CHECK(result.rest.empty());
			CHECK(!result.error_message);
			CHECK(result.tokens.size() == 1);

			CHECK(result.tokens[0].first == tokenizer_t::token_type::kIdentifier);
			CHECK(result.tokens[0].second == fmt::format("{}", entry));
		}
	}
}
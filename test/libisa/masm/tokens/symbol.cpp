#include "catch.hpp"

#include "masm/frontend/tokens.hpp"

TEST_CASE( "Symbol Declarations", "[masm::tokens]"  ) {
	using tokenizer_t = masm::frontend::pep_tokenizer<masm::frontend::lexer_t>;
	tokenizer_t lexer;

	SECTION("Alphabetic Symbols") {
		std::vector<std::string> codes = {"Hello" "WoRlD", "ident_ifiers"};
		for(auto entry : codes)
		{
			auto result = masm::frontend::tokenize<tokenizer_t>(fmt::format("   {}:   ", entry), lexer);
			CHECK(result.success);
			CHECK(result.rest.empty());
			CHECK(!result.error_message);
			CHECK(result.tokens.size() == 1);

			CHECK(result.tokens[0].first == tokenizer_t::token_type::kSymbolDecl);
			CHECK(result.tokens[0].second == fmt::format("{}", entry));
		}
	}
}
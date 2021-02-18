#include "catch.hpp"

#include "masm/frontend/tokens.hpp"
#include "masm/frontend/tokenizer.hpp"
#include "asmb/pep10/tokenizer.hpp"

TEST_CASE( "Macro Arguments", "[asmb::pep10::tokens]"  ) {
	
	using tokenizer_t = asmb::pep10::tokenizer<masm::frontend::lexer_t>;
	tokenizer_t lexer;

	SECTION("Alphabetic identifiers") {
		std::vector<std::string> codes = {"1", "2"};
		for(auto entry : codes)
		{
			auto result = masm::frontend::tokenize<tokenizer_t>(fmt::format("   ${}   ", entry), lexer);
			CHECK(result.success);
			CHECK(result.rest.empty());
			CHECK(!result.error_message);
			CHECK(result.tokens.size() == 1);

			CHECK(result.tokens[0].first == tokenizer_t::token_type::kMacroArg);
			CHECK(result.tokens[0].second == fmt::format("{}", entry));
		}
	}
}
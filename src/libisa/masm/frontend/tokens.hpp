#pragma once

#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_algorithm.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/phoenix.hpp>
#include <fmt/format.h>
namespace masm::frontend
{
using namespace boost::spirit::qi;
using namespace boost::spirit::ascii;
namespace lex = boost::spirit::lex;

typedef lex::lexertl::token<std::string::iterator, boost::mpl::vector<std::string, long long>> token_t;

typedef lex::lexertl::actor_lexer<token_t> lexer_t;
static const std::string inner_lit = {R"(([\w ]|\\([{}bfnrtv\\]|[xX][0-9a-fA-F]{{2}})))"};
template <typename Lexer>
struct pep_tokenizer : lex::lexer<Lexer>
{
	enum class token_type
	{
        kError, kComma, kCharConstant, kComment, kDecConstant, kDotDirective,
        kEmpty, kHexConstant, kIdentifier, kStrConstant, kSymbolDecl,
        kMacroInvoke, kMacroArg
	};

	pep_tokenizer():
		macro_ident("@[A-Za-z_]+"),
		macro_arg("\\$[0-9]+"),
		comma(","),
		identifier("[A-Za-z|_]+"),
		sym_decl("[A-Za-z|_]+:"),
		dot_command("\\.[a-zA-Z]+"),
		ch_lit(fmt::format("'{}'", fmt::format(inner_lit, "'"))),
		str_lit(fmt::format(R"(\"{}+\")", fmt::format(inner_lit, "\""))),
		comment(";.*"),
		hex_literal("0[xX][0-9a-fA-F]+"),
		dec_literal("[+-]{0,1}[0-9]+"),
		empty("\\n"),
		whitespaces("[( \\t)]+")
	{
		using boost::spirit::_1;
		using boost::phoenix::construct;
		std::cout << ch_lit;
		this->self += 
		whitespaces   [(lex::_pass = lex::pass_flags::pass_ignore)]
		| macro_ident [(lex::_tokenid = (int) token_type::kMacroInvoke, lex::_val = construct<std::string>(lex::_start + 1, lex::_end))]
		| sym_decl	  [(lex::_tokenid = (int) token_type::kSymbolDecl, lex::_val = construct<std::string>(lex::_start, lex::_end-1))]
		| dot_command [(lex::_tokenid = (int) token_type::kDotDirective, lex::_val = construct<std::string>(lex::_start+1, lex::_end))]
		| macro_arg   [(lex::_tokenid = (int) token_type::kMacroArg, lex::_val = construct<std::string>(lex::_start + 1, lex::_end))]
		| comma       [ lex::_tokenid = (int) token_type::kComma]
		| hex_literal [ lex::_tokenid = (int) token_type::kHexConstant]
		| dec_literal [ lex::_tokenid = (int) token_type::kDecConstant]
		| str_lit     [(lex::_tokenid = (int) token_type::kStrConstant, lex::_val = construct<std::string>(lex::_start + 1, lex::_end - 1))]
		| ch_lit      [(lex::_tokenid = (int) token_type::kCharConstant, lex::_val = construct<std::string>(lex::_start + 1, lex::_end - 1))]
		| identifier  [ lex::_tokenid = (int) token_type::kIdentifier]
		| empty       [(lex::_tokenid = (int) token_type::kEmpty, lex::_val = "")]
		| comment     [(lex::_tokenid = (int) token_type::kComment, lex::_val = construct<std::string>(lex::_start + 1, lex::_end))];
	}

	lex::token_def<std::string> macro_ident, macro_arg, comma, identifier, sym_decl, dot_command, 
		ch_lit, str_lit, empty, comment, hex_literal, dec_literal;
	lex::token_def<lex::omit> whitespaces;

	token_type convert_token_to_type(token_t token)
	{
		auto id = token.id();
		if(id == -1) return token_type::kError;
		else return static_cast<token_type>(id);
	}
};

template <typename token_enum_t>
struct tokenize_result
{
	bool success = true;
	std::optional<std::string> error_message;
	std::string rest = "";
	std::vector<std::pair<token_enum_t, std::string> > tokens;
};

template<typename tokenizer_t>
tokenize_result<typename tokenizer_t::token_type> tokenize(std::string input, tokenizer_t& tokenizer)
{
	auto first = input.begin(), last=input.end();
	typename tokenizer_t::iterator_type iter = tokenizer.begin(first, last);
	typename tokenizer_t::iterator_type end = tokenizer.end();

	tokenize_result<typename tokenizer_t::token_type> ret;
	while (iter != end) {
		if(!token_is_valid(*iter)) {
			ret.success = false;
			ret.error_message = "Failed to parse token.";
			break;
		}

		std::string s;
		boost::spirit::traits::assign_to(*iter, s);
		auto get_token_type = tokenizer.convert_token_to_type(*iter);
		ret.tokens.emplace_back(std::pair<typename tokenizer_t::token_type, std::string>{get_token_type, s});
		iter++;
	}
	ret.rest = std::string(first, last);
	return ret;
}

}
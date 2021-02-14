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

static const std::string inner_lit = {R"( ([\w ]|\\(['"bfnrtv\\]|[xX][0-9a-f]{2})) )"};
template <typename Lexer>
struct tokenizer : lex::lexer<Lexer>
{
	tokenizer():
	macro_ident("@[A-Za-z_]+"),
	macro_arg("\\$[0-9]+"),
	comma(","),
	identifier("[A-Za-z|_]+"),
	sym_decl("[A-Za-z|_]:"),
	dot_command("\\.[a-zA-Z]"),
	ch_lit(fmt::format("'{}'", inner_lit)),
	str_lit(fmt::format(R"(\"{}+\")", inner_lit)),
	comment(";.*"),
	hex_literal("0[xX][0-9a-fA-F]+"),
	dec_literal("[01]?[0-9]+"),
	empty("\\n"),
	whitespaces("[( \\t)]")
	{
		using boost::spirit::_1;
		using boost::phoenix::construct;
		this->self += whitespaces [lex::_pass = lex::pass_flags::pass_ignore]
		| macro_ident [lex::_val = construct<std::string>(lex::_start + 1, lex::_end)]
		| sym_decl[lex::_val = construct<std::string>(lex::_start, lex::_end-1)]
		| dot_command
		| macro_arg [lex::_val = construct<std::string>(lex::_start + 1, lex::_end)]
		| comma
		| hex_literal
		| dec_literal
		| str_lit [lex::_val = construct<std::string>(lex::_start + 1, lex::_end + 1)]
		| ch_lit [lex::_val = construct<std::string>(lex::_start + 1, lex::_end + 1)]
		| identifier
		| empty [lex::_val = ""]
		| comment [lex::_val = construct<std::string>(lex::_start + 1, lex::_end)];
	}
	lex::token_def<std::string> macro_ident, macro_arg, comma, identifier, sym_decl, dot_command, 
		ch_lit, str_lit, empty, comment, hex_literal, dec_literal;
	lex::token_def<lex::omit> whitespaces;
};
}
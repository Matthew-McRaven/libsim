#pragma once

#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_algorithm.hpp>
#include <boost/spirit/include/phoenix_core.hpp>

namespace masm::frontend
{
using namespace boost::spirit::qi;
using namespace boost::spirit::ascii;
namespace lex = boost::spirit::lex;

template <typename Lexer>
struct tokenizer : lex::lexer<Lexer>
{
	tokenizer():
	macro_ident("@[A-Za-z_]{1}(\\w*)"),
	macro_arg("\\$[0-9]+"),
	comma(","),
	identifier("[A-Za-z|_]"),
	sym_decl("[A-Za-z|_]:"),
	dot_command("\\.[a-zA-Z]\\w*"),
	//ch_lit("\'([^\'\\\\]|\\\\(\'|[bfnrtv]|\"|////)|\\\\[xX][0-9a-fA-F]{2})\'"),
	//str_lit("((\")((([^\"\\\\])|((\\\\)([\'bfnrtv\"\\\\]))|((\\\\)(([x|X])([0-9A-Fa-f]{2}))))*)(\"))"),
	empty(""),
	comment(";.*"),
	hex_literal("0[xX][0-9a-fA-F]+"),
	dec_literal("[+|-]{0,1}[0-9]+")
	{
		this->self	
			= macro_ident //[lex::_val = boost::phoenix::construct<std::string>(lex::_start + 1, lex::_end)]
			| sym_decl //[lex::_val = boost::phoenix::construct<std::string>(lex::_start, lex::_end-1)]
			| dot_command //[lex::_val = boost::phoenix::construct<std::string>(lex::_start + 1, lex::_end)]
			| macro_arg
			| comma
			| hex_literal
			| dec_literal
			| ch_lit
			| str_lit
			| identifier
			| empty
			| comment
			;
	}
	lex::token_def<std::string> macro_ident, macro_arg, comma, identifier, sym_decl, dot_command, 
		ch_lit, str_lit, empty, comment;
	lex::token_def<long long> hex_literal, dec_literal;
};
}
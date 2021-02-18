
#pragma once

#include <tuple>
#include <string>

#include <boost/spirit/include/lex_lexertl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_algorithm.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/phoenix.hpp>

#include "masm/project/project.hpp"
#include "masm/project/image.hpp"

namespace masm::frontend
{
    
template<typename address_size_t, typename tokenizer_t>
class tokenizer
{
public:
    struct flags{};
    tokenizer() {}
    tokenizer(const flags& flag) {}
    virtual ~tokenizer() = default;
    virtual auto tokenize(
        std::shared_ptr<masm::project::project<address_size_t> >& project, 
        std::shared_ptr<masm::elf::code_section<address_size_t> >& section
    ) -> bool;
private:
	tokenizer_t tokenizer_ = {};
};


namespace lex = boost::spirit::lex;

typedef lex::lexertl::token<std::string::iterator, boost::mpl::vector<std::string, long long>> token_t;
typedef lex::lexertl::actor_lexer<token_t> lexer_t;

template <typename token_enum_t>
using token_pair_t =  std::pair<token_enum_t, std::string>;

template <typename token_enum_t>
using tokenized_line_t = std::vector<token_pair_t<token_enum_t> >;

template <typename token_enum_t>
struct tokenize_result
{
	bool success = true;
	std::optional<std::string> error_message;
	std::string rest = "";
	tokenized_line_t<token_enum_t> tokens;
};


template<typename tokenizer_t>
tokenize_result<typename tokenizer_t::token_type> tokenize(std::string input, tokenizer_t& tokenizer);

template<typename token_enum_t = masm::frontend::token_type>
std::pair<bool, token_pair_t<token_enum_t> > match(
    typename tokenized_line_t<token_enum_t>::iterator& first,  const typename tokenized_line_t<token_enum_t>::iterator& end, 
    const std::set<masm::frontend::token_type>, bool advance_iterator
);

template<typename token_enum_t = masm::frontend::token_type>
std::pair<bool, token_pair_t<token_enum_t> > match_not(
    typename tokenized_line_t<token_enum_t>::iterator& first, const typename tokenized_line_t<token_enum_t>::iterator& end, 
    const std::set<masm::frontend::token_type>, bool advance_iterator
);
};
#include "masm/frontend/tokenizer.tpp"
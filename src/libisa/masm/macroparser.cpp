#include "macroparser.hpp"
#include <string>
#include <tuple>
#include <regex>
#include <string_view>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <iostream>

namespace detail
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;


    struct macro
    {
        std::string name;
        int arg_count;
    };
    template <typename Iterator>
    struct macro_parser : qi::grammar<Iterator, macro(), ascii::space_type>
    {
        macro_parser() : macro_parser::base_type(start)
        {
            using qi::int_;
            using qi::lit;
            using qi::double_;
            using ascii::char_;
            using qi::lexeme;
            using qi::no_skip;

            start %=
                   qi::no_skip['@']
                >> qi::lexeme[ +(qi::char_ - ascii::space) ]
                >> int_
                ;
        }
        qi::rule<Iterator, macro(), ascii::space_type> start;
    };
};



BOOST_FUSION_ADAPT_STRUCT(
    ::detail::macro,
    (std::string, name)
    (int, arg_count)
)

std::tuple<bool, std::string, uint8_t> masm::analyze_macro_definition(std::string macro_text)
{
    /*
     * A macro file must begin with with name of the macro, followed by an arbitrary number of spaces
     * followed by an integer in [0,16] specifying the number of arguments the macro takes.
     *
     * Neither comments nor whitespace may occur before this definition line.
     *
     * Below are valid examples:
     * @DECI 2
     * @UNOP 0
     *
     * Below are invalid examples, with comments descrbing why.
     *
     * Whitepace preceding macro definiton:
     *      @DECI 2
     * Space between macro name and macro symbol @.
     * @ deci 2
     *
     * Line ends in a comment
     * @deci 2 ;My comment
     *
     */
    std::string first_line = macro_text.substr(0, macro_text.find("\n"));
    detail::macro macro;
    using boost::spirit::ascii::space;
    using iterator_type =  std::string::const_iterator;
    using macro_parser = detail::macro_parser<iterator_type>;
    macro_parser g;
    auto cbegin = first_line.cbegin();
    bool r = boost::spirit::qi::phrase_parse(cbegin, first_line.cend(), g, 
        boost::spirit::ascii::space, macro);
    // If we failed, or if we did not consume the entire input, fail.
    if(!r || cbegin != first_line.cend()) {
        return {false, "", 0};
    }
    return {true, macro.name, macro.arg_count};
}
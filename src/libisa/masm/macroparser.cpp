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

namespace client
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;


    struct employee
    {
        std::string name;
        int arg_count;
    };
};

namespace client
{

    template <typename Iterator>
    struct employee_parser : qi::grammar<Iterator, employee(), ascii::space_type>
    {
        employee_parser() : employee_parser::base_type(start)
        {
            using qi::int_;
            using qi::lit;
            using qi::double_;
            using ascii::char_;
            using qi::lexeme;

            start %=
                   '@'
                >> qi::lexeme[ +(qi::char_ - ascii::space) ]
                >> int_
                ;
        }
        qi::rule<Iterator, employee(), ascii::space_type> start;
    };
    //]
}

BOOST_FUSION_ADAPT_STRUCT(
    client::employee,
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

    client::employee emp;
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef client::employee_parser<iterator_type> employee_parser;
    employee_parser g;
    bool r = boost::spirit::qi::phrase_parse(macro_text.cbegin(), macro_text.cend(), g, boost::spirit::ascii::space, emp);
    if(!r) {
        return {false, "", 0};
    }

    return {true, emp.name, emp.arg_count};
}
#include "preprocesser.hpp"

// For parser
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
/*
namespace masm::frontend::detail
{
struct invoke_def
{
    std::string name;
	std::vector<std::string> args;
};
}

BOOST_FUSION_ADAPT_STRUCT(
    masm::frontend::detail::invoke_def,
    (std::string, name)
	(std::vector<std::string>, args)
)

namespace masm::frontend::detail
{

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template <typename Iterator>
struct invoke_parser : qi::grammar<Iterator, invoke_def(), ascii::space_type>
{

	invoke_parser() : invoke_parser::base_type(start)
	{
		using qi::lit;
		using qi::lexeme;
		using ascii::char_;
		using boost::spirit::ascii::no_case;
		using boost::spirit::no_skip;

		start %=
				qi::no_skip['@']
			>> qi::lexeme[ +(qi::char_ - ascii::space) ]
			>> qi::lexeme[ +(qi::char_ - ascii::space) -"$"-"@"] % ","
			;
	}

	qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
	qi::rule<Iterator, invoke_def(), ascii::space_type> start;
};
//]
};

void masm::frontend::parse(std::string line)
{
		// Set up our line parser.
		using boost::spirit::ascii::space;
		typedef std::string::const_iterator iterator_type;
		masm::frontend::detail::invoke_parser<iterator_type> parser;

		// Parse only macro lines!!!
		std::cout << line << std::endl;
		auto iter = line.cbegin(), end = line.cend();
		masm::frontend::detail::invoke_def macro;
        bool r = phrase_parse(iter, end, parser, space, macro);
}
*/
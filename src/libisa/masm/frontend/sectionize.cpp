#include "sectionize.hpp"

#include <algorithm>

#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp> // Include for boost::split

// For parser
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
namespace masm::frontend::detail
{
struct section_def
{
    std::string section_name;
};
}

BOOST_FUSION_ADAPT_STRUCT(
    masm::frontend::detail::section_def,
    (std::string, section_name)
)

namespace masm::frontend::detail
{

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template <typename Iterator>
struct section_parser : qi::grammar<Iterator, section_def(), ascii::space_type>
{

	section_parser() : section_parser::base_type(start)
	{
		using qi::lit;
		using qi::lexeme;
		using ascii::char_;
		using boost::spirit::ascii::no_case;
		using boost::spirit::no_skip;

		// Don't ignore whitespace within quotation!!!
		quoted_string %= lexeme['"' >> +(char_ - '"' - ' ') >> '"'];

		start %=
			// We don't care about case for our section declarations.
			no_case[lit(".section")]
			>>  quoted_string;
	}

	qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
	qi::rule<Iterator, section_def(), ascii::space_type> start;
};
//]
};

std::vector<masm::frontend::section_text> masm::frontend::sectionize(const std::string& program, const std::string& default_section)
{
	std::vector<std::string> as_lines;
	// Courtesy of:
	// 	https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
	boost::split(as_lines, program, boost::is_any_of("\n"));

	// Prevent an erronious extra (blank) line from being added to our document.
	if(as_lines.back() == "") {
		as_lines.pop_back();
	}
	// Re-added \n to make tokenizer happier.s
	for(auto& line : as_lines) line.append("\n");
	
	// Set up our line parser.
	using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef masm::frontend::detail::section_parser<iterator_type> section_parser;
	section_parser parser;

	int line_index=0;
	std::list<std::tuple<size_t,std::string>> section_matches;

	// Insert default section. The default section starts at the first line of the document.
	section_matches.emplace_back(std::tuple<size_t,std::string>({0, default_section}));

	// Check each line for a .SECTION directive.
	for(const auto& line : as_lines) {
        masm::frontend::detail::section_def section;
        auto iter = line.begin(), end = line.end();
        bool r = phrase_parse(iter, end, parser, space, section);
		if (r && iter == end) {
			section_matches.emplace_back(std::tuple<size_t,std::string>({line_index, section.section_name}));
		}
		line_index++;
	}

	// Pre-allocate room for all matches.
	std::vector<masm::frontend::section_text> ret{};
	ret.reserve(section_matches.size());

	// Split source document's lines into sections.
	// Iterate from end, so that we can do easy iterator math with no additional lookahead/lookbehind.
	auto end = as_lines.end();
	while(!section_matches.empty()) {
		auto [index, name] = section_matches.back();
		section_matches.pop_back();


		masm::frontend::section_text active_section = {index, name, {}};
		// Copy lines after init, so that we can use std::copy instead of rolling our own.
		std::copy(as_lines.begin() + index, end, std::back_inserter(active_section.lines));

		// Only add section if it is non-empty.
		// Only section at risk of being empty is default section, since it is implicit and may span no
		// lines if the first line of code is a .SECTION directive.
		if(!active_section.lines.empty()) {
			ret.push_back(active_section);
		}
		

		end = as_lines.begin() + index;
	}

	// Re-sort return value so that sections are stored in the order in which they appear in the source document.
	using sec_t = masm::frontend::section_text;
	std::sort(ret.begin(), ret.end(), [](const sec_t& lhs, const sec_t& rhs){
		return lhs.document_line<rhs.document_line;}
	);

	return ret;
}
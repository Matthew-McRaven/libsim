#include "sectionize.hpp"
#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp> // Include for boost::split
std::list<masm::frontend::section_text> masm::frontend::sectionize(std::string program, std::string default_section)
{
	std::vector<std::string> as_lines;
	// Courtesy of:
	// 	https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
	boost::split(as_lines, program, boost::is_any_of("\n"), boost::token_compress_on);
}
#include "macroparser.hpp"
#include <string>
#include <tuple>
#include <regex>
#include <string_view>

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
	auto reg = std::regex("\\s+");
	std::smatch match;
	std::regex_match(first_line.cbegin(), first_line.cend(), match, reg);

    if(match.size() != 2) {
        // Malformed macro declaration.
        return {false, "", 0};
    }

    auto name = match[0].str();
    if(name[0] != '@') {
        // Name must begin with @
        return {false, "", 0};
    }

    
	char* end;
	auto text = match[1].str();
	auto arg_count = strtol(text.data(), &end, 10);
    
    if(end == nullptr) {
        // Arg count was not an integer.
        return {false, "", 0};
    }
    return {true, name.substr(1), arg_count};
}
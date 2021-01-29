#include "macroparser.hpp"
#include <string>
#include <tuple>
#include <regex>
#include <string_view>

#include <iostream>

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

    return {false, "", 0};

}
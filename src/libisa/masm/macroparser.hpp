#pragma once

#include <tuple>
#include <string>
#include <climits>

namespace masm {
// Analyze a macro's text body, and attempt to extract header information.
// Tuple returns 1) is the header well formed, 2) what is the macro's name,
// 3) how many arguments does the macro require?
std::tuple<bool, std::string, uint8_t> analyze_macro_definition(std::string macro_text);
}; // End namespace masm
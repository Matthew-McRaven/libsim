#pragma once 
#include <climits>

namespace symbol {
// Format for symbols
enum class SymbolReprFormat: int
{
    kNone, kChar, kDec, kHex
};

}; //end namespace symbol

#include "types.tpp"
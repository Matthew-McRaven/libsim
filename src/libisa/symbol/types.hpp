#pragma once 
#include <climits>

namespace symbol {
// Format for symbols
enum class SymbolReprFormat: int
{
    kNone, kChar, kDec, kHex
};

enum class definition_state
{
    kSingle, kMultiple, kUndefined
};

enum class type
{
    kEmpty, kLocation, kConstant, kPtrToSym
};

enum class binding
{
    kLocal,
    kGlobal,
    kWeak,
};

}; //end namespace symbol

#include "types.tpp"
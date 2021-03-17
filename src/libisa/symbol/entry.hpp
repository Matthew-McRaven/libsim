#pragma once

#include <climits>
#include <memory>
#include <string>

#include "symbol/types.hpp"


namespace symbol {
    
template<typename value_t>
class table;

struct format
{
    SymbolReprFormat format;
    uint32_t size = 0;
};
/*
 * A symbol can either be:
 *  1) Undefined: A symbol is not defined, and referenced 1+ times.
 *  2) Singlely Defined: A symbol is defined once, and referenced 0+ times.
 *  3) Multiply Defined: A symbol is defined 2+ times, and referenced 0+ times.
 */

/*
 * A symbol entry represents one named symbol from a microprogram.
 * Symbols have multiple definition states that allow the microassembler to error if symbols are defined incorrectly.
 * Symbols' value can either be empty (i.e. undefined) or store the address of a line of microcode.
 * The symbol will automatically adjust between SINGLE and UNDEFINED based on the value pased to setValue(...),
 * but the class will not automatically set its definition state to MULTIPLE.
 * The function setMultiplyDefined() is the only way to notify a Symbol that it has been defined more than once.
 *
 * This function should be invoked from whatever assembler of compiler created the SymbolEntry - the SymbolTable will not do this for you.
 * This is because symbols could constantly be updated and changed (i.e. code relocation), so it would be very difficult for this class to decide if it has
 * been defined more than once, instead of simply being updated in place.
 */
template<typename value_t>
class entry
{

public:
    //Default constructor, assumes value is SymbolEmpty
    entry(typename symbol::table<value_t>& parent, typename symbol::table<value_t>::ID ID, std::string name);
    entry(typename symbol::table<value_t>& parent, typename symbol::table<value_t>::ID ID,
        std::string name, typename symbol::table<value_t>::value_ptr_t value);
    ~entry() = default;
    type type() const {return value->type();}

    // Non-owning reference to containing symbol table.
    typename symbol::table<value_t> const &  parent;
    // Unique identifier describing this symbol
    typename symbol::table<value_t>::ID const ID;

    // Unique string name of symbol as appearing in the sources
    std::string name;
    definition_state state;
    binding binding;
    typename symbol::table<value_t>::value_ptr_t value;
};

} // end namespace symbol

#include "entry.tpp"
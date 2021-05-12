#pragma once

#include <climits>
#include <memory>
#include <string>

#include "symbol/types.hpp"


namespace symbol {
    
template<typename value_t>
class table;

// Currently unused, will eventuallly be used to track trace tag information alongside a symbol.
// TODO: Determine how to track debugging information.
struct format
{
    SymbolReprFormat format;
    uint32_t size = 0;
};


/*!
 * \brief A symbol represents a named value or location in memory that may be defined at most once.
 * 
 * Symbols provided by this namespace are meant to be flexibile--supporting microprogrammin and ELF usage.
 * 
 * Symbols that are undefined after linkage should cause an error; symbols defined multiple times should cause
 * immediate errors.
 * Some examples of symbol values are: currently undefined (symbol::empty), an address of a line of code
 * (symbol::value_location), or a numeric constant (symbol::value_const)
 * 
 * As this class exposes all of its data publicly, it is up to the user to ensure that properties such as binding
 * and definition state are updated correctly.
 * This API decision was made because it is impossible for a symbol to know if its value is being set because it is defined,
 * or if the value is being set to handle relocation.
 * Simply put, the symbol doesn't know enough to update these fields.
 * 
 * \tparam value_t An unsigned integral type that is large enough to contain the largest address on the target system.
 */
template<typename value_t>
class entry
{

public:
    //Default constructor, assumes value is symbol::value_empty
    entry(typename symbol::table<value_t>& parent, std::string name);
    entry(typename symbol::table<value_t>& parent, std::string name, typename symbol::table<value_t>::value_ptr_t value);
    ~entry() = default;

    //! Non-owning reference to containing symbol table.
    typename symbol::table<value_t> const &  parent;

    //! Unique name as appearing in source code.
    std::string name;
    //! Keep track of how many times this symbol's name has been defined.
    definition_state state;
    /*! The binding type of this symbol (i.e., global vs local).
     * \sa symbol::binding*/
    binding_t binding;
    /*! The value taken on by this symbol.
     * \sa symbol::abstract_value */
    typename symbol::table<value_t>::value_ptr_t value;
};

} // end namespace symbol

#include "entry.tpp"
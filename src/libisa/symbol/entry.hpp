#pragma once

#include <climits>
#include <memory>
#include <string>

#include "symbol/types.hpp"


namespace symbol {
template<typename T>
class SymbolTable;
/*
 * A symbol can either be:
 *  1) Undefined: A symbol is not defined, and referenced 1+ times.
 *  2) Singlely Defined: A symbol is defined once, and referenced 0+ times.
 *  3) Multiply Defined: A symbol is defined 2+ times, and referenced 0+ times.
 */
enum class DefStates
{
    SINGLE, MULTIPLE, UNDEFINED
};

struct SymbolFormat
{
    SymbolReprFormat format;
    uint32_t size = 0;
};

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
template<typename symbol_value_t>
class SymbolEntry
{
private:
    // Unique identifier describing this symbol
    typename symbol::SymbolTable<symbol_value_t>::SymbolID symbolID;
    // Unique string name of symbol as appearing in the sources
    std::string name;
    typename symbol::SymbolTable<symbol_value_t>::AbstractSymbolValuePtr symbolValue;
    DefStates definedState;
    // Non-owning pointer to parent. DO NOT DELETE.
    typename symbol::SymbolTable<symbol_value_t>* parent;
public:
    //Default constructor, assumes value is SymbolEmpty
    SymbolEntry(typename symbol::SymbolTable<symbol_value_t>* parent, typename symbol::SymbolTable<symbol_value_t>::SymbolID symbolID,
        std::string name);
    SymbolEntry(typename symbol::SymbolTable<symbol_value_t>* parent, typename symbol::SymbolTable<symbol_value_t>::SymbolID symbolID,
        std::string name, typename symbol::SymbolTable<symbol_value_t>::AbstractSymbolValuePtr value);
    ~SymbolEntry();
    const typename symbol::SymbolTable<symbol_value_t>* getParentTable() const;
    void setValue(typename symbol::SymbolTable<symbol_value_t>::AbstractSymbolValuePtr value);
    //Get the string name of the symbol
    std::string getName() const;

    bool isDefined() const;
    bool isUndefined() const;
    bool isMultiplyDefined() const;
    void setMultiplyDefined();
    // Allow a symbol's definition state to be managed by a symbol table.
    void setDefinedState(DefStates state);

    typename symbol::SymbolTable<symbol_value_t>::SymbolID getSymbolID() const;
    //Uses the internal data pointer, and returns the value of its getValue() method
    symbol_value_t getValue() const;
    //Returns the internal data pointer, in case one wishes to access its other methods
    typename symbol::SymbolTable<symbol_value_t>::AbstractSymbolValuePtr getRawValue();
};

template <typename symbol_value_t>
bool SymbolAlphabeticComparator(std::shared_ptr<const SymbolEntry<symbol_value_t>> &lhs, std::shared_ptr<const SymbolEntry<symbol_value_t>> &rhs);
} // end namespace symbol

#include "entry.tpp"
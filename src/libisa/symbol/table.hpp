#pragma once

#include <atomic>
#include <memory>
#include <map>
#include <mutex>
#include <vector>
#include <type_traits>

#include "symbol/helper.hpp"
#include "symbol/entry.hpp"

namespace symbol {

template<typename value_t>
class AbstractSymbolValue;

/*
 * The SymbolTable class provides lookups base on the names and unique identifiers of a group of SymbolEntries.
 * A SymbolEntry is created by calling insertSymbol(...), and can then be looked up by name or by its unique identifier.
 *
 */

template <typename offset_size_t>
	//requires (UnsignedIntegral<offset_t>)
class SymbolTable
{
public:
    // This type uniquely identifies a SymbolEntry within a symbol table.
    // It is not gaurenteed to be unique across runs or between multiple SymbolTable instances at runtime.
    using SymbolID = uint16_t;
    // Convenience typdefs of commonly used templated types to reduce code verbosity.
    using SymbolEntryPtr = std::shared_ptr<symbol::SymbolEntry<offset_size_t> >;
    using AbstractSymbolValuePtr = std::shared_ptr<symbol::AbstractSymbolValue<offset_size_t> >;

private:
    static SymbolID nextUserSymbolID;
    static SymbolID getNextUserSymbolID();
    std::vector<SymbolEntryPtr> externalSymbols;
    std::map<SymbolID, SymbolEntryPtr> symbolDictionary;
    std::map<std::string, SymbolID> symbolLookup;

public:
    explicit SymbolTable();
    ~SymbolTable();
    // Return a symbol entry by name or ID. Returns an empty shared_ptr if the symbol being looked for doesn't exist.
    SymbolEntryPtr getValue(SymbolID symbolID) const;
    SymbolEntryPtr getValue(const std::string& symbolName) const;
    // Create a new SymbolEntry with the passed name.
    SymbolEntryPtr insertSymbol(const std::string& symbolName);
    // Change the value of a SymbolEntry.
    // If the symbol already exists, it will be be set to multiple defined.
    // If one wants to update a symbol value in place, getValue(...) on the symbol,
    // and setValue(...) on that symbol.
    SymbolEntryPtr setValue(SymbolID symbolID, AbstractSymbolValuePtr value);
    SymbolEntryPtr setValue(const std::string& symbolName, AbstractSymbolValuePtr value);

    // Indicate that you are referencing a (supposedly) declared symbol.
    // If the symbol is not in the symbol table, it will bre created.
    SymbolEntryPtr reference(const std::string& symbolName);
    // Indicate that you are defining a new symbol. If the symbol
    // already exists, the symbol will be flagged as multiply defined.
    SymbolEntryPtr define(const std::string& symbolName);
    // Declare a symbol as external, allowing it to be used in other translation units.
    void declareExternal(const std::string& symbolName);
    // Return the list of symbols that may be linked externally.
    auto getExternalSymbols() const -> std::vector<const SymbolEntryPtr>;

    // Check if a symbol exists.
    bool exists(const std::string& symbolName) const;
    bool exists(SymbolID symbolID) const;
    // Get the count of symbols that have definition problems.
    uint32_t numMultiplyDefinedSymbols() const;
    uint32_t numUndefinedSymbols() const;

    // Set the offset of all relocatable symbols with an address
    // above threshhold to value.
    void setOffset(offset_size_t value, offset_size_t threshhold = 0);
    // Set the offset of all relocatable symbols to 0.
    void clearOffset();
    auto getSymbolEntries() const -> std::vector<const SymbolEntryPtr>;
    // Return the map of all symbolic definitions. Used to provide access to iterators
    // to perform custom operations and comparisions over all symbols.
    const std::map<SymbolID, SymbolEntryPtr> getSymbolMap() const;

    std::string getSymbolTableListing() const;
};
}; //end namespace symbol
#include "table.tpp"

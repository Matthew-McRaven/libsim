#pragma once

#include <atomic>
#include <memory>
#include <map>
#include <mutex>
#include <vector>
#include <type_traits>

#include <boost/range/adaptors.hpp>
#include <boost/range/any_range.hpp>

#include "symbol/entry.hpp"

namespace symbol {

template<typename value_t>
class AbstractSymbolValue;

/*
 * The SymbolTable class provides lookups base on the names and unique identifiers of a group of SymbolEntries.
 * A SymbolEntry is created by calling insertSymbol(...), and can then be looked up by name or by its unique identifier.
 *
 */

template <typename value_t>
	//requires (UnsignedIntegral<offset_t>)
class table
{
public:
    // This type uniquely identifies a SymbolEntry within a symbol table.
    // It is not gaurenteed to be unique across runs or between multiple SymbolTable instances at runtime.
    using ID = uint16_t;
    // Convenience typdefs of commonly used templated types to reduce code verbosity.
    using entry_ptr_t = std::shared_ptr<symbol::entry<value_t> >;
    using value_ptr_t = std::shared_ptr<symbol::abstract_value<value_t> >;

private:
    static ID next_id_;
    static ID generate_new_ID();

    std::map<ID, entry_ptr_t> id_to_entry_;
    std::map<std::string, ID> name_to_id_;

public:
    using range = boost::any_range<entry_ptr_t, boost::forward_traversal_tag, entry_ptr_t&, std::ptrdiff_t>;//decltype(id_to_entry_.begin() | boost::adaptors::map_values);
    using const_range = boost::any_range<const entry_ptr_t, boost::forward_traversal_tag, const entry_ptr_t&, std::ptrdiff_t>;//decltype(id_to_entry_.cbegin() | boost::adaptors::map_values);

    explicit table();
    ~table();

    // Indicate that you are referencing a (supposedly) declared symbol.
    // If the symbol is not in the symbol table, it will be created.
    entry_ptr_t reference(const std::string& symbolName);
    // Indicate that you are defining a new symbol. If the symbol
    // already exists, the symbol will be flagged as multiply defined.
    entry_ptr_t define(const std::string& symbolName);
    // Declare a symbol as external, allowing it to be used in other translation units.

    // Modify ST_INFO fields of an ELF symbol.
    void set_binding(const std::string& name, binding binding);
    void set_type(const std::string& name, type type);

    // Check if a symbol exists.
    bool exists(const std::string& name) const;
    bool exists(ID ID) const;

    auto entries() const -> const_range;
    auto entries() -> range;

    std::string listing() const;
};

    // Return the list of symbols that may be linked externally.
    template <typename symbol_value_t>
    auto externals(typename table<symbol_value_t>::const_range rng) -> decltype(rng);

    // Get the count of symbols that have definition problems.
    template <typename symbol_value_t>
    size_t count_multiply_defined_symbols(typename table<symbol_value_t>::const_range rng);
    template <typename symbol_value_t>
    size_t count_undefined_symbols(typename table<symbol_value_t>::const_range rng);

    // Set the offset of all relocatable symbols with an address
    // above threshhold to value.
    template <typename symbol_value_t>
    void set_offset(typename table<symbol_value_t>::range rng, symbol_value_t offset, symbol_value_t threshhold = 0);

    // Set the offset of all relocatable symbols to 0.
    template <typename symbol_value_t>
    void clear_offset(typename table<symbol_value_t>::range rng);

}; //end namespace symbol
#include "table.tpp"

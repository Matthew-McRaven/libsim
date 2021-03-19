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


/*!
 * \brief A symbol::table represents a collection of symbols that are contained within the same translation unit.
 * \tparam value_t An unsigned integral type that is large enough to contain the largest address on the target system.
 */

template <typename value_t>
	//requires (UnsignedIntegral<offset_t>)
class table
{
public:
    // Convenience typdefs of commonly used templated types to reduce code verbosity.

    //! The type of a pointer to a symbol.
    using entry_ptr_t = std::shared_ptr<symbol::entry<value_t> >;
    //! The type of a pointer to a symbol's value.
    using value_ptr_t = std::shared_ptr<symbol::abstract_value<value_t> >;

private:
    std::map<std::string, entry_ptr_t> name_to_entry_;

public:
    /*! \brief The type representing an iterable collection of mutable symbol entries.
    Very useful for custom transformations and filters on a table's symbols.
    */
    using range = boost::any_range<entry_ptr_t, boost::forward_traversal_tag, entry_ptr_t&, std::ptrdiff_t>;
    /*! \brief The type representing an iterable collection of constant symbol entries.
    Very useful for custom transformations and filters on a table's symbols.
    */
    // TODO: Fix Use pointer to const rather than a const ptr
    using const_range = boost::any_range<const entry_ptr_t, boost::forward_traversal_tag, const entry_ptr_t&, std::ptrdiff_t>; 

    explicit table();
    ~table() = default;

    /*! \brief Get a symbol::entry from this symbol table by name look-up.
     * 
     * If the symbol is not yet in the symbol table, a new entry will be added, and it will be marked as undefined.
     * If the symbol exists, then a pointer to the existing symbol will be returned.
     * 
     * All calls to reference in the same table with the same name will return pointers to the same underlying entry.
     * 
     * \arg name The name of the symbol to be defined.
     * \returns Returns a pointer to a symbol whose name matches the argument exactly.
     */
    entry_ptr_t reference(const std::string& name);

    /*! \brief Reference a symbol by name, and advance its definition state.
     * 
     * Calls symbol::table::reference(name) on the passed symbol.
     * Advancing definition state takes symbols in state symbol::definition_state::kUndefined to symbol::definition_state::kSingle.
     * It also takes symbols in state symbol::definition_state::kSingle to symbol::definition_state::kMultiple.
     * Symbols in symbol::definition_state::kMultiple remain multiply defined.
     * 
     * \arg name The name of the symbol to be defined.
     * \returns Returns the pointer to the now-defined symbol.
     * \sa reference
     */
    entry_ptr_t define(const std::string& name);

    /*! \brief Remove a symbol from this table by name.
     * 
     * Un-registers the symbol from any internal maps, and marks the symbol for deletion.
     * If the passed symbol is not present in the table, the operation is a no-op that returns true.
     * 
     * \arg name The name of the symbol to be marked for deletion.
     * \returns Return true if there are no remaining references to "name" and false otherwise.
     */
    bool del(const std::string& name);

    /*! \brief Modify ST_INFO fields of an ELF symbol.
     * 
     * Changes the symbol::binding of the named symbol.
     * If the passed symbol does not exist, it will be created and its type will be set to type.
     * \arg name The name of the symbol whose binding is to be modified.
     * \arg type The new binding of the symbol.
     * 
     * \sa symbol::binding
     */
    void set_binding(const std::string& name, binding binding);

    /*! \brief Modify ST_INFO fields of an ELF symbol.
     * 
     * Changes the symbol::type of the named symbol.
     * If the passed symbol does not exist, it will be created and its type will be set to type.
     * \arg name The name of the symbol whose type is to be modified.
     * \arg type The new type of the symbol.
     * 
     * \sa symbol::type
     */
    void set_type(const std::string& name, type type);

    /*! \brief Check if a symbol is present in this table.
     * \arg name The name of the symbol to check for the existence ofl
     * \returns Returns true if a symbol with name "name" is present in the table, and false otherwise.
     */
    bool exists(const std::string& name) const;

    /*! \brief Return a constant range of all symbols in the table.
     * Useful for gathering data to perform operations such as finding all global symbols.
     * \returns Returns a constant range containing all symbols in the table.
     */
    auto entries() const -> const_range;

    /*! \brief Return a mutable range of all symbols in the table.
     * Useful for bulk modification of symbols, such as performing symbol relocation.
     * \returns Returns a mutable range containing all symbols in the table.
     */
    auto entries() -> range;

    /*! \brief Converts the symbol table to a "pretty printed" string.
     * This pretty print should be the exact format expected by our listing pane in Pep/10.
     * \returns Returns a string representation of the symbol table
     */
    // TODO: Migrate listing to a non-member method
    std::string listing() const;
};

    /*! \brief Filter a collection of symbols for those marked as global.
     * \arg rng A collection of symbols to be filtered.
     * \tparam value_t An unsigned integral type that is large enough to contain the largest address on the target system.
     * \returns Returns a collection of symbols that whose binding is marked as symbol::binding::kGlobal.
     */
    template <typename symbol_value_t>
    auto externals(typename table<symbol_value_t>::const_range rng) -> decltype(rng);

    /*! \brief Count the number of multiply defined symbols in a collection.
     * \arg rng A collection of symbols to be filtered.
     * \tparam value_t An unsigned integral type that is large enough to contain the largest address on the target system.
     * \returns The number of symbols in rng whose definition_state is marked as symbol::definition_state::kMultiple.
     */
    template <typename symbol_value_t>
    size_t count_multiply_defined_symbols(typename table<symbol_value_t>::const_range rng);
    
    /*! \brief Count the number of undefined symbols in a collection.
     * \arg rng A collection of symbols to be filtered.
     * \tparam value_t An unsigned integral type that is large enough to contain the largest address on the target system.
     * \returns The number of symbols in rng whose definition_state is marked as symbol::definition_state::kUndefined.
     */
    template <typename symbol_value_t>
    size_t count_undefined_symbols(typename table<symbol_value_t>::const_range rng);

    /*! \brief Relocate symbols by setting the offset of symbols whose base address exceeds the threshold.
     * 
     * This functions performs the hard work of relocation.
     * It filters out symbols that are not relocatable, and for symbols that are (symbol::value_location),
     * it conditionally sets the offset field, effecitvely relocating the symbol.
     * 
     * \arg rng A collection of symbols to be relocated.
     * \arg offset Value to which the symbols' offsets should be set.
     * \arg threshold Base address which symbol must exceed or equal to be eligible for relocation.
     * \tparam value_t An unsigned integral type that is large enough to contain the largest address on the target system.
     * \returns The number of symbols in rng whose definition_state is marked as symbol::definition_state::kUndefined.
     */
    template <typename symbol_value_t>
    void set_offset(typename table<symbol_value_t>::range rng, symbol_value_t offset, symbol_value_t threshhold = 0);

    /*! \brief Clear the offsets of all relocatable symbols, undoing the work of relocation.
     * 
     * "Undo" relocation by calling set_offset with (rng, 0, 0). 
     * 
     * \arg rng A collection of symbols to be relocated.
     * \tparam value_t An unsigned integral type that is large enough to contain the largest address on the target system.
     * \returns The number of symbols in rng whose definition_state is marked as symbol::definition_state::kUndefined.
     */
    template <typename symbol_value_t>
    void clear_offset(typename table<symbol_value_t>::range rng);

}; //end namespace symbol
#include "table.tpp"

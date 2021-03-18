// File: symboltable.cpp
/*
    The Pep/9 suite of applications (Pep9, Pep9CPU, Pep9Micro) are
    simulators for the Pep/9 virtual machine, and allow users to
    create, simulate, and debug across various levels of abstraction.

    Copyright (C) 2018 J. Stanley Warford & Matthew McRaven, Pepperdine University

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "symbol/table.hpp"

#include <fmt/core.h>
#include <utility>
#include <stdexcept>

#include "symbol/entry.hpp"
#include "symbol/value.hpp"

template <typename T>
typename symbol::table<T>::ID symbol::table<T>::next_id_ = {0};

template <typename symbol_value_t>
typename symbol::table<symbol_value_t>::ID symbol::table<symbol_value_t>::generate_new_ID()
{
    return ++next_id_;
}

template <typename symbol_value_t>
symbol::table<symbol_value_t>::table() = default;

template <typename symbol_value_t>
symbol::table<symbol_value_t>::~table() = default;

template <typename symbol_value_t>
typename symbol::table<symbol_value_t>::entry_ptr_t 
    symbol::table<symbol_value_t>::reference(const std::string& name)
{
    if(auto index = name_to_id_.find(name); index == name_to_id_.end()) {
        // Must create symbol and put in correct tables.
        auto id = generate_new_ID();
        name_to_id_[name] = id;
        auto ret = std::make_shared<entry<symbol_value_t>>(*this, id, name);
        id_to_entry_[id] = ret;
        return ret;
    }
    else return id_to_entry_[index->second];
}

template <typename symbol_value_t>
typename symbol::table<symbol_value_t>::entry_ptr_t 
    symbol::table<symbol_value_t>::define(const std::string& name)
{
    auto entry = reference(name);
    if(entry->state == definition_state::kUndefined) entry->state = definition_state::kSingle;
    else if(entry->state == definition_state::kSingle) entry->state = definition_state::kMultiple;
    return entry;
}

template <typename symbol_value_t>
bool symbol::table<symbol_value_t>::del(const std::string& name)
{
    // Symbol does not exist, so the delete succeded.
    if(auto index = name_to_id_.find(name); index == name_to_id_.end()) return true;
    auto entry = reference(name);
    id_to_entry_.erase(entry->ID);
    name_to_id_.erase(entry->name);
    if(entry.use_count() == 1) return true;
    else {
        entry->value = std::make_shared<symbol::value_deleted<symbol_value_t>>();
        entry->state = symbol::definition_state::kUndefined;
        return false;
    }
}

template <typename symbol_value_t>
void symbol::table<symbol_value_t>::set_binding(const std::string &name, symbol::binding binding)
{
    auto symbol = reference(name);
    symbol->binding = binding;
}

template <typename symbol_value_t>
void symbol::table<symbol_value_t>::set_type(const std::string &name, symbol::type type)
{
    auto symbol = reference(name);
    symbol->type = type;
}

template <typename symbol_value_t>
bool symbol::table<symbol_value_t>::exists(const std::string& name) const
{
    return name_to_id_.find(name) != name_to_id_.end();
}

template <typename symbol_value_t>
bool symbol::table<symbol_value_t>::exists(ID id) const
{
    return id_to_entry_.find(id) != id_to_entry_.end();
}

template <typename offset_size_t>
auto symbol::table<offset_size_t>::entries() const -> symbol::table<offset_size_t>::const_range
{
    return id_to_entry_ | boost::adaptors::map_values;
}

template <typename offset_size_t>
auto symbol::table<offset_size_t>::entries() -> symbol::table<offset_size_t>::range
{
    return id_to_entry_ | boost::adaptors::map_values;
}

template <typename offset_size_t>
std::string symbol::table<offset_size_t>::listing() const
{

    static const std::string line = "--------------------------------------\n";
    static const std::string symTableStr = "Symbol table\n";
    static const std::string headerStr = "Symbol    Value        Symbol    Value\n";
    std::string build;
    /*auto list = getSymbolEntries();
    //std::sort(list.begin(),list.end(), SymbolAlphabeticComparator);

    // Don't generate an empty symbol table.
    if(list.isEmpty()) {
        return "";
    }*/
    throw std::logic_error("Not yet implemented");
    // TODO: finish implementation.
    /*for(auto it = list.begin(); it != list.end(); ++it) {
        if(it + 1 ==list.end()) {
            QString hexString = QString("%1").arg((*it)->getValue(), 4, 16, QLatin1Char('0')).toUpper();
            build.append(QString("%1%2\n").arg((*it)->getName(), -10).arg(hexString, -13));
        }
        else {
            QString hexString = QString("%1").arg((*it)->getValue(), 4, 16, QLatin1Char('0')).toUpper();
            build.append(QString("%1%2").arg((*it)->getName(), -10).arg(hexString, -13));
            ++it;
            hexString = QString("%1").arg((*it)->getValue(), 4, 16, QLatin1Char('0')).toUpper();
            build.append(QString("%1%2\n").arg((*it)->getName(), -10).arg(hexString, -13));
        }
    }
    return symTableStr % line % headerStr %line % build % line;*/
    return "";
}



template <typename symbol_value_t>
auto symbol::externals(typename symbol::table<symbol_value_t>::const_range rng) -> decltype(rng)
{
    static const auto filter = [](const auto& it){return it->binding == symbol::binding::kGlobal;};
    return rng | boost::adaptors::filtered(filter);
}

template <typename symbol_value_t>
size_t symbol::count_multiply_defined_symbols(typename symbol::table<symbol_value_t>::const_range rng)
{
    auto it = rng.begin();
    uint32_t count = 0;
    while(it != rng.end()) {
        count += it->isMultiplyDefined() ? 1 : 0;
        it++;
    }
    return count;
}

template <typename symbol_value_t>
size_t symbol::count_undefined_symbols(typename symbol::table<symbol_value_t>::const_range rng)
{
    auto it = rng.cbegin();
    uint32_t count = 0;
    while(it != rng.end()) {
        count += it->isUndefined() ? 1 : 0;
        it++;
    }
    return count;
}

template <typename symbol_value_t>
void symbol::set_offset(typename symbol::table<symbol_value_t>::range rng, symbol_value_t offset, symbol_value_t threshhold)
{
    auto it = rng.begin();
    while(it != rng.end()) {
        if((*it)->value->type() == symbol::type::kLocation && (*it)->value->value() >= threshhold) {
            std::static_pointer_cast<symbol::value_location<symbol_value_t>>((*it)->value)->set_offset(offset);
        }
        ++it;
    }
}

template <typename symbol_value_t>
void symbol::clear_offset(typename symbol::table<symbol_value_t>::range rng)
{
    // Clearing offsets is the same thing as setting all offsets to 0.
    set_offset(rng, 0, 0);
}
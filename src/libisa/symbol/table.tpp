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
typename symbol::SymbolTable<T>::SymbolID symbol::SymbolTable<T>::nextUserSymbolID = {0};

template <typename offset_size_t>
typename symbol::SymbolTable<offset_size_t>::SymbolID symbol::SymbolTable<offset_size_t>::getNextUserSymbolID()
{
    return ++nextUserSymbolID;
}

template <typename offset_size_t>
symbol::SymbolTable<offset_size_t>::SymbolTable() = default;

template <typename offset_size_t>
symbol::SymbolTable<offset_size_t>::~SymbolTable() = default;

template <typename offset_size_t>
typename symbol::SymbolTable<offset_size_t>::SymbolEntryPtr 
    symbol::SymbolTable<offset_size_t>::getValue(SymbolID symbolID) const
{
    if(auto index = symbolDictionary.find(symbolID); index == symbolDictionary.end()) return nullptr;
    else return index->second;
}

template <typename offset_size_t>
typename symbol::SymbolTable<offset_size_t>::SymbolEntryPtr 
    symbol::SymbolTable<offset_size_t>::getValue(const std::string & symbolName) const
{
    if(auto index = symbolLookup.find(symbolName); index == symbolLookup.end())  return nullptr;
    else return getValue(index->second);
}

template <typename offset_size_t>
typename symbol::SymbolTable<offset_size_t>::SymbolEntryPtr 
    symbol::SymbolTable<offset_size_t>::insertSymbol(const std::string & symbolName)
{
    // We don't want multiple symbols to exists in the same table with the same name.
    if(exists(symbolName)) return getValue(symbolName);
    SymbolID id = SymbolTable::getNextUserSymbolID();
    symbolLookup[symbolName] = id.load();
    symbolDictionary[id] = std::make_shared<SymbolEntry>(this, id.load(), symbolName);
    return symbolDictionary[id];
}

template <typename offset_size_t>
typename symbol::SymbolTable<offset_size_t>::SymbolEntryPtr 
symbol::SymbolTable<offset_size_t>::setValue(SymbolID symbolID, AbstractSymbolValuePtr value)
{
    SymbolEntryPtr rval = symbolDictionary[symbolID];
    // If the symbol has already been defined, this function vall constitutes a redefinition.
    if(rval->isDefined()) {
        rval->setMultiplyDefined();
    }
    rval->setValue(std::move(value));
    return rval;
}

template <typename offset_size_t>
typename symbol::SymbolTable<offset_size_t>::SymbolEntryPtr 
    symbol::SymbolTable<offset_size_t>::setValue(const std::string& symbolName, AbstractSymbolValuePtr value)
{
    // If the table doesn't contain a symbol, create it first.
    if(!exists(symbolName)) insertSymbol(symbolName);
    return setValue(symbolLookup.find(symbolName)->second, std::move(value));
}

template <typename offset_size_t>
typename symbol::SymbolTable<offset_size_t>::SymbolEntryPtr 
    symbol::SymbolTable<offset_size_t>::reference(const std::string& symbolName)
{
    if(auto index = symbolLookup.find(symbolName); index == symbolLookup.end()) return insertSymbol(symbolName);
    else return getValue(index->second);
}

template <typename offset_size_t>
typename symbol::SymbolTable<offset_size_t>::SymbolEntryPtr 
    symbol::SymbolTable<offset_size_t>::define(const std::string& symbolName)
{
    auto index = symbolLookup.find(symbolName);
    SymbolTable::SymbolEntryPtr entry;
    if(index == symbolLookup.end()) entry = insertSymbol(symbolName);
    else entry = getValue(index->second);
    // Defining a symbol "increases" the definition state by one.
    if(entry->isUndefined()) entry->setDefinedState(DefStates::SINGLE);
    else if(entry->isDefined()) entry->setDefinedState(DefStates::MULTIPLE);
    return entry;
}

template <typename offset_size_t>
void symbol::SymbolTable<offset_size_t>::declareExternal(const std::string &symbolName)
{
    // an EXPORT statement does not declare a symbol,
    // so therefore we are referencing one that already exists.
    externalSymbols.push_back(reference(symbolName));
}

template <typename offset_size_t>
auto symbol::SymbolTable<offset_size_t>::getExternalSymbols() const 
    -> std::vector<const SymbolEntryPtr>
{

    return this->externalSymbols;
}

template <typename offset_size_t>
bool symbol::SymbolTable<offset_size_t>::exists(const std::string& symbolName) const
{
    return symbolLookup.find(symbolName) != symbolLookup.end();
}

template <typename offset_size_t>
bool symbol::SymbolTable<offset_size_t>::exists(SymbolID symbolID) const
{
    return symbolDictionary.find(symbolID) != symbolDictionary.end();
}

template <typename offset_size_t>
uint32_t symbol::SymbolTable<offset_size_t>::numMultiplyDefinedSymbols() const
{
    uint32_t count = 0;
    for(const auto& ptr : this->symbolDictionary) {
        count += ptr->isMultiplyDefined() ? 1 : 0;
    }
    return count;
}

template <typename offset_size_t>
uint32_t symbol::SymbolTable<offset_size_t>::numUndefinedSymbols() const
{
    uint32_t count = 0;
    for(const auto& ptr : this->symbolDictionary) {
        count += ptr->isUndefined() ? 1 : 0;
    }
    return count;
}

template <typename offset_size_t>
void symbol::SymbolTable<offset_size_t>::setOffset(offset_size_t value, offset_size_t threshhold)
{
    for(SymbolEntryPtr ptr : this->symbolDictionary) {
        if(ptr->getRawValue()->getSymbolType() == SymbolType::ADDRESS && ptr->getValue() >= threshhold) {
            static_cast<SymbolValueLocation<offset_size_t>*>(ptr->getRawValue().data())->setOffset(value);
        }
    }
}

template <typename offset_size_t>
void symbol::SymbolTable<offset_size_t>::clearOffset()
{
    // Clearing offsets is the same thing as setting all offsets to 0.
    setOffset(0, 0);
}

template <typename offset_size_t>
auto symbol::SymbolTable<offset_size_t>::getSymbolEntries() const
    -> std::vector<const SymbolEntryPtr>
{
    return symbolDictionary.values();
}

template <typename offset_size_t>
const std::map<typename symbol::SymbolTable<offset_size_t>::SymbolID, typename symbol::SymbolTable<offset_size_t>::SymbolEntryPtr> 
    symbol::SymbolTable<offset_size_t>::getSymbolMap() const
{
    return symbolDictionary;
}

template <typename offset_size_t>
std::string symbol::SymbolTable<offset_size_t>::getSymbolTableListing() const
{

    static const std::string line = "--------------------------------------\n";
    static const std::string symTableStr = "Symbol table\n";
    static const std::string headerStr = "Symbol    Value        Symbol    Value\n";
    std::string build;
    auto list = getSymbolEntries();
    std::sort(list.begin(),list.end(), SymbolAlphabeticComparator);

    // Don't generate an empty symbol table.
    if(list.isEmpty()) {
        return "";
    }
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

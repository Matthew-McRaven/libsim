// File: symbolentry.cpp
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

#include "symbol/entry.hpp"
#include "symbol/value.hpp"

template<typename symbol_value_t>
symbol::SymbolEntry<symbol_value_t>::SymbolEntry(symbol::SymbolTable<symbol_value_t>* parent, 
    typename symbol::SymbolTable<symbol_value_t>::SymbolID symbolID, std::string name): 
    symbolID(symbolID), name(name),
    symbolValue(std::shared_ptr<SymbolValueEmpty<symbol_value_t> >::create()), definedState(DefStates::UNDEFINED), parent(parent)
{
}

template<typename symbol_value_t>
symbol::SymbolEntry<symbol_value_t>::SymbolEntry(symbol::SymbolTable<symbol_value_t>* parent, 
    typename symbol::SymbolTable<symbol_value_t>::SymbolID symbolID,
    std::string name, typename symbol::SymbolTable<symbol_value_t>::AbstractSymbolValuePtr value): 
    symbolID(symbolID), name(name), symbolValue(nullptr), parent(parent)
{
    setValue(value);
}

template<typename symbol_value_t>
symbol::SymbolEntry<symbol_value_t>::~SymbolEntry()
{

}

template<typename symbol_value_t>
const symbol::SymbolTable<symbol_value_t>* symbol::SymbolEntry<symbol_value_t>::getParentTable() const
{
    return parent;
}

template<typename symbol_value_t>
void symbol::SymbolEntry<symbol_value_t>::setValue(typename symbol::SymbolTable<symbol_value_t>::AbstractSymbolValuePtr value)
{
    // A SymbolEntry will not transfer from SINGLE to MULTIPLE on its own.
    // This is because a symbol table / assembler might need to update the value
    // of the symbol in place to achieve code relocation, and so the responsibility
    // to make that decision is delegated to owning objects.
	symbolValue = value;
    // If given an empty value, then the symbol is undefined
    if (dynamic_cast<SymbolValueEmpty<symbol_value_t>*>(value.data())) {
        definedState = DefStates::UNDEFINED;
	}
    //If the symbol is multiply defined, it remains multiply defined
    else if(definedState == DefStates::MULTIPLE) {
        definedState = DefStates::MULTIPLE;
    }
    else {
        definedState = DefStates::SINGLE;
	}
}

template<typename symbol_value_t>
std::string symbol::SymbolEntry<symbol_value_t>::getName() const
{
	return name;
}

template<typename symbol_value_t>
bool symbol::SymbolEntry<symbol_value_t>::isDefined() const
{
    return definedState == DefStates::SINGLE;
}

template<typename symbol_value_t>
bool symbol::SymbolEntry<symbol_value_t>::isUndefined() const
{
    return definedState == DefStates::UNDEFINED;
}

template<typename symbol_value_t>
bool symbol::SymbolEntry<symbol_value_t>::isMultiplyDefined() const
{
	return definedState == DefStates::MULTIPLE;
}

template<typename symbol_value_t>
void symbol::SymbolEntry<symbol_value_t>::setMultiplyDefined()
{
    definedState = DefStates::MULTIPLE;
}

template<typename symbol_value_t>
void symbol::SymbolEntry<symbol_value_t>::setDefinedState(DefStates state)
{
    this->definedState = state;
}

template<typename symbol_value_t>
typename symbol::SymbolTable<symbol_value_t>::SymbolID symbol::SymbolEntry<symbol_value_t>::getSymbolID() const
{
	return symbolID;
}

template<typename symbol_value_t>
symbol_value_t symbol::SymbolEntry<symbol_value_t>::getValue() const
{
	return symbolValue->getValue();
}

template<typename symbol_value_t>
typename symbol::SymbolTable<symbol_value_t>::AbstractSymbolValuePtr symbol::SymbolEntry<symbol_value_t>::getRawValue()
{
    return symbolValue;
}

template<typename symbol_value_t>
bool SymbolAlphabeticComparator(std::shared_ptr<const symbol::SymbolEntry<symbol_value_t>> &lhs, 
    std::shared_ptr<const symbol::SymbolEntry<symbol_value_t> > &rhs)
{
    return lhs->getName() < rhs->getName();
}

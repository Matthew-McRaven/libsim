// File: symbolvalue.cpp
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

#include "symbol/value.hpp"
#include "symbol/entry.hpp"

template <typename value_t>
symbol::AbstractSymbolValue<value_t>::AbstractSymbolValue()
{
}

template <typename value_t>
symbol::AbstractSymbolValue<value_t>::~AbstractSymbolValue()
{

}

template <typename value_t>
symbol::SymbolValueEmpty<value_t>::SymbolValueEmpty(): AbstractSymbolValue<value_t>()
{
}

template <typename value_t>
symbol::SymbolValueEmpty<value_t>::~SymbolValueEmpty()
{
}

template <typename value_t>
value_t symbol::SymbolValueEmpty<value_t>::getValue() const
{
    return value_t();
}

template <typename value_t>
symbol::SymbolType symbol::SymbolValueEmpty<value_t>::getSymbolType() const
{
    return SymbolType::EMPTY;
}

template <typename value_t>
symbol::SymbolValueNumeric<value_t>::SymbolValueNumeric(value_t value): value(value)
{

}


template <typename value_t>
symbol::SymbolValueNumeric<value_t>::~SymbolValueNumeric()
{

}

template <typename value_t>
void symbol::SymbolValueNumeric<value_t>::setValue(value_t value)
{
    this->value = value;
}


template <typename value_t>
value_t symbol::SymbolValueNumeric<value_t>::getValue() const
{
    return value;
}

template <typename value_t>
symbol::SymbolType symbol::SymbolValueNumeric<value_t>::getSymbolType() const
{
    return SymbolType::NUMERIC_CONSTANT;
}

template <typename value_t>
symbol::SymbolValueLocation<value_t>::SymbolValueLocation(value_t value):AbstractSymbolValue<value_t>(), base(value), offset(0)
{
}

template <typename value_t>
symbol::SymbolValueLocation<value_t>::~SymbolValueLocation()
{
}

template <typename value_t>
void symbol::SymbolValueLocation<value_t>::setBase(value_t value)
{
    this->base = value;
}

template <typename value_t>
void symbol::SymbolValueLocation<value_t>::setOffset(value_t value)
{
    this->offset = value;
}

template <typename value_t>
value_t symbol::SymbolValueLocation<value_t>::getValue() const
{
    return base + offset;

}

template <typename value_t>
symbol::SymbolType symbol::SymbolValueLocation<value_t>::getSymbolType() const
{
    return SymbolType::ADDRESS;
}

template <typename value_t>
bool symbol::SymbolValueLocation<value_t>::canRelocate() const
{
    return true;
}

template <typename value_t>
value_t symbol::SymbolValueLocation<value_t>::getOffset() const
{
    return offset;
}

template <typename value_t>
value_t symbol::SymbolValueLocation<value_t>::getBase() const
{
    return base;
}

template <typename value_t>
symbol::SymbolValueExternal<value_t>::SymbolValueExternal(std::shared_ptr<const SymbolEntry<value_t> >symbol): 
    symbol(std::move(symbol))
{

}

template <typename value_t>
symbol::SymbolValueExternal<value_t>::~SymbolValueExternal() = default;

template <typename value_t>
value_t symbol::SymbolValueExternal<value_t>::getValue() const
{
    return symbol->getValue();
}

template <typename value_t>
symbol::SymbolType symbol::SymbolValueExternal<value_t>::getSymbolType() const
{
    return SymbolType::EXTERNAL;
}

template <typename value_t>
bool symbol::SymbolValueExternal<value_t>::canRelocate() const
{
    // We should not allow relocation of a symbol defined in
    // another translation unit.
    return false;
}

template <typename value_t>
std::shared_ptr<const symbol::SymbolEntry<value_t> >symbol:: SymbolValueExternal<value_t>::getSymbolValue()
{
    return symbol;
}

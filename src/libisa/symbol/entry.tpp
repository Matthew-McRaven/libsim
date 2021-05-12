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


template<typename value_t>
symbol::entry<value_t>::entry(symbol::table<value_t>& parent, std::string name): 
    parent(parent),
    state(definition_state::kUndefined), name(name), binding(binding_t::kLocal),
    value(std::make_shared<symbol::value_empty<value_t>>())
{
}

template<typename value_t>
symbol::entry<value_t>::entry(symbol::table<value_t>& parent, 
    std::string name, typename symbol::table<value_t>::value_ptr_t value): 
    parent(parent),
    state(definition_state::kSingle), name(name), binding(binding_t::kLocal), value(value)
{
}

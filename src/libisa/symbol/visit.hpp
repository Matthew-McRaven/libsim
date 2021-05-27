#pragma once

// File: visit.hpp
/*
    The Pep/10 suite of applications (Pep10, Pep10CPU, Pep10Term) are
    simulators for the Pep/10 virtual machine, and allow users to
    create, simulate, and debug across various levels of abstraction.

    Copyright (C) 2021 J. Stanley Warford & Matthew McRaven, Pepperdine University

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

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <variant>

#include <boost/range/adaptors.hpp>
#include <boost/range/any_range.hpp>

#include "entry.hpp"
#include "table.hpp"

namespace symbol
{

// Find the root of the symbol tree.
template<typename value_t>
struct RootVisitor
{
	NodeType<value_t> operator() (std::shared_ptr<BranchTable<value_t>> table);
	NodeType<value_t> operator() (std::shared_ptr<LeafTable<value_t>> table);
};

// Gather all symbols sharing the same name into a list.
template<typename value_t>
class SelectByNameVisitor
{
public:
	SelectByNameVisitor(std::string name);
	std::list<std::shared_ptr<symbol::entry<value_t>>> return_val;
	void operator()(std::shared_ptr<BranchTable<value_t>> table);
	void operator() (std::shared_ptr<LeafTable<value_t>> table);
private:
	std::string target;
};


// Check for the existence of a symbol by name
template<typename value_t>
class ExistenceVisitor
{
public:
	ExistenceVisitor(std::string name);
	
	bool operator() (std::shared_ptr<BranchTable<value_t>> table);
	bool operator() (std::shared_ptr<LeafTable<value_t>> table);
private:
	std::string target;
};

/*
 * Helper methods that wrap visitor creation and std::visit invocation.
 */

/*!
 * \brief Find the root table in a hierarchy given any of its descendants or itself.
 * \arg table A node in a hierarchical symbol table.
 * \returns Returns the input table if that table has no parent, or it's greatest ancestor in the parent exists.
 * \tparam value_t An unsigned integral type that is large enough to contain the largest address on the target system.
 * \sa symbol::RootVisitor
 */
template<typename value_t>
symbol::NodeType<uint16_t> root_table(NodeType<value_t> table);

template<typename value_t>
std::list<std::shared_ptr<symbol::entry<value_t>>> select_by_name(const std::string& name, NodeType<value_t> table);

/*!
 * \brief Determine if any table in the hierarchical symbol table contains a symbol with a particular name
 * \arg table A node in a hierarchical symbol table.
 * \arg name The name of the symbol to be found.
 * \returns Returns true if at least one child of table contains
 * \tparam value_t An unsigned integral type that is large enough to contain the largest address on the target system.
 * \sa symbol::ExistenceVisitor
 */
template<typename value_t>
bool exists(const std::string& name, NodeType<value_t> table);

} //end namespace symbol

#include "visit.tpp"
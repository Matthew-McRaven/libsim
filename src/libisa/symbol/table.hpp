#pragma once

// File: table.hpp
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
#include <variant>
#include <map>
#include <string>
#include <list>
#include <optional>
#include <boost/range/adaptors.hpp>
#include <boost/range/any_range.hpp>

#include "entry.hpp"

namespace symbol {

template<typename value_t>
class BranchTable;
template<typename value_t>
class LeafTable;

/*
 * Some implementation ideas drawn from: 
 * Design and Implementation of the Symbol Table for Object-Oriented Programming Language,
 * Yangsun Lee 2017, http://dx.doi.org/10.14257/ijdta.2017.10.7.03
*/
template<typename value_t>
using NodeType = std::variant<std::shared_ptr<symbol::BranchTable<value_t>>, 
	std::shared_ptr<symbol::LeafTable<value_t>> >;

template<typename value_t>
class BranchTable : public std::enable_shared_from_this<BranchTable<value_t>>
{
public:
	explicit BranchTable() = default;
	explicit BranchTable(std::shared_ptr<BranchTable<value_t>> parent);
	~BranchTable() = default;

	void add_child(NodeType<value_t> child);
	std::list<NodeType<value_t>> children(){return children_;}
	const std::weak_ptr<BranchTable<value_t>> parent_ = {};
private:
	std::list<NodeType<value_t>> children_;
};

template<typename value_t>
class LeafTable : public std::enable_shared_from_this<LeafTable<value_t>>
{
public:
	using entry_ptr_t = std::shared_ptr<symbol::entry<value_t>>;
	using range = boost::any_range<entry_ptr_t, boost::forward_traversal_tag, entry_ptr_t&, std::ptrdiff_t>;
	using const_range = boost::any_range<const entry_ptr_t, boost::forward_traversal_tag, const entry_ptr_t&, std::ptrdiff_t>; 
	
	explicit LeafTable() = default;
	explicit LeafTable(std::shared_ptr<BranchTable<value_t>> parent);
	~LeafTable() = default;

	//!< Unlike reference, get() will not create an entry in the table if the symbol fails to be found.
	std::optional<entry_ptr_t> get(const std::string& name) const;
	entry_ptr_t reference(const std::string& name);
	entry_ptr_t define(const std::string& name);

	//!< Once a symbol has been marked as global, there is no un-global'ing it. 
	//!< This function handles walking the tree and pointing other local symbols to this tables global instance.
	void mark_global(const std::string& name);
	bool exists(const std::string& name) const;

	/*
	 * Return all symbols contained by the table.
	 */
	auto entries() const -> const_range;
	auto entries() -> range;

	const std::weak_ptr<BranchTable<value_t>> parent_ = {};
private:
	std::map<std::string, entry_ptr_t> name_to_entry_;
};

//!< Wrap the creation of a new BranchTable and the correct setup of the parent/child relationship.
template <typename value_t>
std::shared_ptr<LeafTable<value_t>> insert_leaf(std::shared_ptr<BranchTable<value_t>> parent);

//!< Wrap the creation of a new LeafTable and the correct setup of the parent/child relationship.
template <typename value_t>
std::shared_ptr<BranchTable<value_t>> insert_branch(std::shared_ptr<BranchTable<value_t>> parent);

} //end namespace symbol
#include "table.tpp"
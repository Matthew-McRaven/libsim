// File: visit.tpp
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

#include "visit.hpp"

/*
 * RootVisitor Implementation
 */
template<typename value_t>
symbol::NodeType<value_t> symbol::RootVisitor<value_t>::operator() (std::shared_ptr<symbol::BranchTable<value_t>> table)
{
	if(auto parent = table->parent_.lock(); parent) return std::visit(*this, NodeType<value_t>(parent));
	else return NodeType<value_t>(table);
}

template<typename value_t>
symbol::NodeType<value_t> symbol::RootVisitor<value_t>::operator() (std::shared_ptr<symbol::LeafTable<value_t>> table)
{
	if(auto parent = table->parent_.lock(); parent) return std::visit(*this, NodeType<value_t>(parent));
	else return NodeType<value_t>(table);
}

/*
 * SelectByNameVisitor Implementation
 */
template<typename value_t>
symbol::SelectByNameVisitor<value_t>::SelectByNameVisitor(std::string name): target(name)
{}

template<typename value_t>
void symbol::SelectByNameVisitor<value_t>::operator() (std::shared_ptr<symbol::BranchTable<value_t>> table)
{
	for(auto& child : table->children()) {
		std::visit(*this, NodeType<value_t>(child));
	}
}

template<typename value_t>
void symbol::SelectByNameVisitor<value_t>::operator() (std::shared_ptr<symbol::LeafTable<value_t>> table)
{
	if(auto maybe_symbol = table->get(target); maybe_symbol) return_val.push_back(maybe_symbol.value());
}

/*
 * ExistenceVisitor Implementation
 */
template<typename value_t>
symbol::ExistenceVisitor<value_t>::ExistenceVisitor(std::string name): target(name)
{}

template<typename value_t>
bool symbol::ExistenceVisitor<value_t>::operator() (std::shared_ptr<symbol::BranchTable<value_t>> table)
{
	auto ret = false;
	for(auto& child : table->children()) ret |= std::visit(*this, NodeType<value_t>(child));
	return ret;
}

template<typename value_t>
bool symbol::ExistenceVisitor<value_t>::operator() (std::shared_ptr<symbol::LeafTable<value_t>> table)
{
	return table->exists(target);
}

/*
 * Helper methods
 */
template<typename value_t>
symbol::NodeType<uint16_t> symbol::root_table(NodeType<value_t> table)
{
	auto vis = symbol::RootVisitor<value_t>();
	return std::visit(vis, table);
}

template<typename value_t>
std::list<std::shared_ptr<symbol::entry<value_t>>>  symbol::select_by_name(const std::string& name, 
	NodeType<value_t> table)
{
	auto root = root_table(table);
	auto vis = symbol::SelectByNameVisitor<value_t>(name);
	std::visit(vis, root);
	return std::move(vis.return_val);
	return {};
}

template<typename value_t>
bool symbol::exists(const std::string& name, NodeType<value_t> table)
{
	auto vis = symbol::ExistenceVisitor<value_t>(name);
	return std::visit(vis, table);
}
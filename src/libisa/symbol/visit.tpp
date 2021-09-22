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
std::list<std::shared_ptr<symbol::entry<value_t>>> symbol::SelectByNameVisitor<value_t>::operator() (std::shared_ptr<symbol::BranchTable<value_t>> table)
{
	auto ret = std::list<std::shared_ptr<symbol::entry<value_t>>>();
	for(auto& child : table->children()) {
		auto child_vals = std::visit(*this, NodeType<value_t>(child));
		ret.splice(ret.end(), child_vals);
	}
	return ret;
}

template<typename value_t>
std::list<std::shared_ptr<symbol::entry<value_t>>> symbol::SelectByNameVisitor<value_t>::operator() (std::shared_ptr<symbol::LeafTable<value_t>> table)
{
	if(auto maybe_symbol = table->get(target); maybe_symbol) return{maybe_symbol.value()};
	return {};
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
 * AdjustOffsetVisitor
 */
template<typename value_t>
symbol::AdjustOffsetVisitor<value_t>::AdjustOffsetVisitor(value_t offset): offset_(offset), threshhold_(0)
{

}
template<typename value_t>
symbol::AdjustOffsetVisitor<value_t>::AdjustOffsetVisitor(value_t offset, value_t threshhold): offset_(offset), threshhold_(threshhold)
{
	
}
	
template<typename value_t>
void symbol::AdjustOffsetVisitor<value_t>::operator() (std::shared_ptr<BranchTable<value_t>> table)
{
	for(auto& child : table->children()) std::visit(*this, NodeType<value_t>(child));
}

template<typename value_t>
void symbol::AdjustOffsetVisitor<value_t>::operator() (std::shared_ptr<LeafTable<value_t>> table)
{
	auto rng = table->entries();
	auto it = rng.begin();
	while(it != rng.end()) {
		if((*it)->value->relocatable() && (*it)->value->value() >= threshhold_) {
			std::static_pointer_cast<symbol::value_location<value_t>>((*it)->value)->set_offset(offset_);
		}
		++it;
	}
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
std::list<std::shared_ptr<symbol::entry<value_t>>>  symbol::select_by_name(NodeType<value_t> table,
	const std::string& name, TraversalPolicy policy)
{
	auto vis = symbol::SelectByNameVisitor<value_t>(name);
	if(policy == TraversalPolicy::kSiblings) table = symbol::parent(table);
	else if(policy == TraversalPolicy::kWholeTree) table = symbol::root_table(table);
	return std::visit(vis, table);
}

template<typename value_t>
bool symbol::exists(NodeType<value_t> table, const std::string& name, TraversalPolicy policy)
{
	auto vis = symbol::ExistenceVisitor<value_t>(name);
	if(policy == TraversalPolicy::kSiblings) table = symbol::parent(table);
	else if(policy == TraversalPolicy::kWholeTree) table = symbol::root_table(table);
	return std::visit(vis, table);
}

template<typename value_t>
void symbol::adjust_offset(NodeType<value_t> table, value_t offset, value_t threshhold, TraversalPolicy policy)
{
	auto vis = symbol::AdjustOffsetVisitor<value_t>(offset, threshhold);
	if(policy == TraversalPolicy::kSiblings) table = symbol::parent(table);
	else if(policy == TraversalPolicy::kWholeTree) table = symbol::root_table(table);
	return std::visit(vis, table);
}
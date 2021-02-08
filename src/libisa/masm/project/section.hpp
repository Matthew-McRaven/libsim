#pragma once

#include <climits>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include <ngraph.hpp>
#include <boost/algorithm/string/classification.hpp> // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp> // Include for boost::split

#include "masm/ir/base.hpp"
#include "masm/registry.hpp"
#include "masm/macro.hpp"

namespace masm::elf {
	template <typename address_size_t>
	class macro_subsection;
}

namespace masm::elf::code {
	struct raw
	{
		std::string text;
		std::vector<std::string> lines;
	};

	template <typename address_size_t>
	struct pre
	{
		std::map<uint16_t, std::shared_ptr<masm::elf::macro_subsection<address_size_t> > > macros;
	};

	template <typename address_size_t>
	struct ir
	{
		std::vector<masm::ir::linear_line<address_size_t> > ir_lines;
		void* stack_trace_info;
	};
}

namespace masm::elf {

struct section_info
{
	std::string name;
	uint16_t index;
	bool read=true, write=true, execute=true;
	uint16_t align=2;
	void* section_type;
};
template <typename address_size_t>
struct code_section {
	section_info header;
	std::optional<code::raw> body_raw;
	std::optional<code::pre<address_size_t> > body_preprocess;
	std::optional<code::ir<address_size_t> > body_ir;
};

template <typename address_size_t>
struct macro_subsection;

template <typename address_size_t>
struct top_level_section : public code_section<address_size_t> {
	NGraph::tGraph<uint32_t> macro_dependency_graph;
	std::map<uint32_t, std::shared_ptr<macro_subsection<address_size_t> > > macro_lookup;

};

template <typename address_size_t>
struct macro_subsection : public code_section<address_size_t> {
	std::weak_ptr<code_section<address_size_t> > direct_parent;
	std::weak_ptr<top_level_section<address_size_t> > containing_section;
	std::vector<std::string> macro_args;
};


/*
 * Helper methods
 */

// Check if a macro with a certain set of args exists withing a top level section.
// TODO: Implement
template <typename address_size_t>
std::optional<std::shared_ptr<macro_subsection<address_size_t> > > macro_from_name(
	std::shared_ptr<top_level_section<address_size_t> > containing_section, 
	std::string macro_name,
	std::optional<std::vector<std::string> > macro_args= std::nullopt);

// Determine which line number in the containing section eventually points to a particular macro
// TODO: Implement
template <typename address_size_t>
	uint32_t map_macro_to_container_line(std::shared_ptr<macro_subsection<address_size_t> > macro);



template <typename address_size_t>
struct link_result
{
	std::optional<std::shared_ptr<macro_subsection<address_size_t> > > section_ptr;
	std::optional<std::string> message;
};

static const std::string kNoSuchMacro = "The macro {} does not exist.";
static const std::string kArgcMismatch = "The macro {} expected {} arguments, but was invoked with {} arguments.";
static const std::string kNoRelation = "The containing_section was unrelated to direct_parent.";

// If possible, register a macro-subsection to a parent.
template <typename address_size_t>
link_result<address_size_t> add_macro_subsection(
	const masm::macro_registry& registry,
	std::shared_ptr<top_level_section<address_size_t>>& containing_section,
	std::shared_ptr<code_section<address_size_t>>& direct_parent,
	uint32_t direct_parent_line_number,
	const std::string& macro_name,
	const std::vector<std::string>& macro_args
)
{
	// If there is parental relationship between the sections, then this whol process makes no sense.
	if(auto k = containing_section->macro_lookup.find(direct_parent->header.index);
		(containing_section != direct_parent)
		&& (k != containing_section->macro_lookup.cend())
		&& (k->second != direct_parent) 
	) return {std::nullopt, kNoRelation};

	// Determine if macro exists, and if it has the same number as args as we are requesting.
	std::optional<decltype(registry.macro({}))> macro = std::nullopt;
	if(registry.contains(macro_name)) macro = registry.macro(macro_name);
	if (!macro) return {std::nullopt, kNoSuchMacro};
	else if(macro_args.size() != macro.value()->arg_count) return {std::nullopt, kArgcMismatch};

	// Create a new macro subsection using the containing_section's dependency graph.
	auto new_section = std::make_shared<macro_subsection<address_size_t>>();
	// Init header.
	new_section->header.name = macro_name;
	new_section->header.index = containing_section->macro_lookup.size();
	// Init macro-specific params
	new_section->containing_section = containing_section;
	new_section->direct_parent = direct_parent;
	new_section->macro_args = macro_args;
	// Copy raw text from
	new_section->body_raw = masm::elf::code::raw();
	new_section->body_raw->text = macro.value()->macro_text;
	// Courtesy of:
	// 	https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
	boost::split(new_section->body_raw->lines, new_section->body_raw->text, boost::is_any_of("\n"), boost::token_compress_on);

	// Link the new section into the dependency graph of the containing section.
	containing_section->macro_lookup[new_section->header.index] = new_section;
	containing_section->macro_dependency_graph.insert_vertex(new_section->header.index);
	containing_section->macro_dependency_graph.insert_edge(direct_parent->header.index, new_section->header.index);

	// Link the direct parent to our newly created macro.
	if(!direct_parent->body_preprocess) direct_parent->body_preprocess = masm::elf::code::pre<address_size_t>();
	direct_parent->body_preprocess->macros[direct_parent_line_number]  = new_section;
	return {new_section, {}};
	
}

}


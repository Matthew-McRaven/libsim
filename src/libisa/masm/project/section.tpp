#include "section.hpp"
template <typename address_size_t>
std::shared_ptr<masm::elf::macro_subsection<address_size_t> > masm::elf::top_level_section<address_size_t>::insert_macro(
	std::shared_ptr<code_section<address_size_t> > direct_parent, uint32_t line_number,
	std::string macro_name, std::vector<std::string> macro_args, std::string macro_text
)
{
	auto index = macro_lookup.size();

	auto child_macro = std::make_shared<masm::elf::macro_subsection<address_size_t>>();
	child_macro->macro_args = macro_args;
	child_macro->direct_parent = direct_parent;
	// If the current section is a macro, then we can assign containing top level section via parent.
	if(auto as_macro = std::dynamic_pointer_cast<masm::elf::macro_subsection<address_size_t> >(direct_parent); as_macro != nullptr) 
		child_macro->containing_section = as_macro->containing_section;
	// Otherwise, we have a top level section, and it is the containing top level section.
	else if(auto as_tls = std::dynamic_pointer_cast<masm::elf::top_level_section<address_size_t> >(direct_parent); as_tls != nullptr) {
		child_macro->containing_section = as_tls;
	}
	child_macro->header.name = macro_name;
	child_macro->header.index = index;
	child_macro->body_raw = masm::elf::code::raw();
	child_macro->body_raw.value().text = macro_text;

	// Courtesy of:
	// 	https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
	boost::split(child_macro->body_raw.value().lines, child_macro->body_raw.value().text, boost::is_any_of("\n"));

	// Prevent an erronious extra (blank) line from being added to our document.
	if(child_macro->body_raw.value().lines.back() == "") {
		child_macro->body_raw.value().lines.pop_back();
	}
	// Re-added \n to make tokenizer happier.
	for(auto line : child_macro->body_raw.value().lines) line.append("\n");

	macro_lookup[index] = child_macro;
	macro_dependency_graph.includes_vertex(index);
	macro_dependency_graph.includes_edge({direct_parent->header.index, index});
	direct_parent->line_to_macro[line_number] = child_macro;

	return child_macro;
}

// If possible, register a macro-subsection to a parent.
template <typename address_size_t>
masm::elf::link_result<address_size_t> masm::elf::add_macro_subsection(
	const masm::macro_registry& registry,
	std::shared_ptr<masm::elf::top_level_section<address_size_t>>& containing_section,
	std::shared_ptr<masm::elf::code_section<address_size_t>>& direct_parent,
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
	) return {std::nullopt, masm::elf::kNoRelation};

	// Determine if macro exists, and if it has the same number as args as we are requesting.
	std::optional<decltype(registry.macro({}))> macro = std::nullopt;
	if(registry.contains(macro_name)) macro = registry.macro(macro_name);
	if (!macro) return {std::nullopt, masm::elf::kNoSuchMacro};
	else if(macro_args.size() != macro.value()->arg_count) return {std::nullopt, masm::elf::kArgcMismatch};

	// Create a new macro subsection using the containing_section's dependency graph.
	auto new_section = std::make_shared<masm::elf::macro_subsection<address_size_t>>();
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
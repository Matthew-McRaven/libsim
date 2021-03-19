#include "section.hpp"

template <typename address_size_t>
typename masm::elf::top_level_section<address_size_t>::invoke_class_t
masm::elf::top_level_section<address_size_t>::add_or_get_invoke_class(
	const std::shared_ptr<code_section<address_size_t> >& section)
{
	if(auto as_macro = std::dynamic_pointer_cast<masm::elf::macro_subsection<address_size_t> >(section); as_macro != nullptr) {
		
		for(auto&[key, value]:macro_def_to_invoke_class) {
			auto& [name, args] = key;
			if(name != as_macro->header.name) continue;
			else if(args != as_macro->macro_args) continue;
			else return value;
		};
		auto next = invoke_dependency_graph.num_nodes()+1;
		macro_def_to_invoke_class[{as_macro->header.name, as_macro->macro_args}] = next;
		invoke_dependency_graph.insert_vertex(next);
		return next;
	}
	// Otherwise, we have a top level section, and it is the containing top level section.
	else if(auto as_tls = std::dynamic_pointer_cast<masm::elf::top_level_section<address_size_t> >(section); as_tls != nullptr) {
		if(!invoke_dependency_graph.includes_vertex(0)) invoke_dependency_graph.insert_vertex(0);
		return 0;
	}
	else throw std::logic_error("");

}

template <typename address_size_t>
std::shared_ptr<masm::elf::macro_subsection<address_size_t> > masm::elf::top_level_section<address_size_t>::insert_macro(
	std::shared_ptr<code_section<address_size_t> > direct_parent, uint32_t line_number,
	std::string macro_name, std::vector<std::string> macro_args, std::string macro_text
)
{

	// Increment size by 1, so that we don't claim parent's index of 0.
	auto index = index_to_macro.size() + 1;

	auto child_macro = std::make_shared<masm::elf::macro_subsection<address_size_t>>();
	child_macro->macro_args = macro_args;
	child_macro->direct_parent = direct_parent;
	// If the current section is a macro, then we can assign containing top level section via parent.
	if(auto as_macro = std::dynamic_pointer_cast<masm::elf::macro_subsection<address_size_t> >(direct_parent); as_macro != nullptr) {
		child_macro->containing_section = as_macro->containing_section;
	}
	// Otherwise, we have a top level section, and it is the containing top level section.
	else if(auto as_tls = std::dynamic_pointer_cast<masm::elf::top_level_section<address_size_t> >(direct_parent); as_tls != nullptr) {
		child_macro->containing_section = as_tls;
	}
	child_macro->header.name = macro_name;
	child_macro->header.index = index;
	child_macro->containing_image = this->containing_image;
	child_macro->body_raw = masm::elf::code::raw();
	child_macro->body_raw.value().text = macro_text;
	child_macro->line_number = line_number;

	// Courtesy of:
	// 	https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
	boost::split(child_macro->body_raw.value().lines, child_macro->body_raw.value().text, boost::is_any_of("\n"));

	// Prevent an erronious extra (blank) line from being added to our document.
	if(child_macro->body_raw.value().lines.back() == "") {
		child_macro->body_raw.value().lines.pop_back();
	}
	// Re-added \n to make tokenizer happier.
	for(auto& line : child_macro->body_raw.value().lines) line.append("\n");

	index_to_macro[index] = child_macro;

	// If the parent doesn't have a node in the graph (possible if parent is a top level module), add it now.
	auto parent_ic = add_or_get_invoke_class(direct_parent);

	// Check if the invocation class of the macro exists.
	auto child_ic = add_or_get_invoke_class(child_macro);
	
	invoke_dependency_graph.insert_edge({parent_ic, child_ic});

	// Link the direct parent to our newly created macro.
	if(!direct_parent->body_preprocess) direct_parent->body_preprocess = masm::elf::code::pre<address_size_t>();
	direct_parent->body_preprocess->macro_line_mapping[line_number]  = child_macro;

	return child_macro;
}
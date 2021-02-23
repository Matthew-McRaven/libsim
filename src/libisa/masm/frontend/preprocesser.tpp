#include "preprocesser.hpp"

#include <boost/range/adaptor/map.hpp>
#include <ngraph_prune.hpp>
#include <ngraph_path.hpp>

#include "tokens.hpp"
#include "masm/frontend/tokenizer.hpp"
template<typename address_size_t, typename tokenizer_t>
masm::frontend::preprocessor<address_size_t, tokenizer_t>::~preprocessor() = default;

template<typename address_size_t, typename tokenizer_t>
auto masm::frontend::preprocessor<address_size_t,tokenizer_t >::preprocess(
	std::shared_ptr<masm::project::project<address_size_t> >& project, 
	std::shared_ptr<masm::elf::code_section<address_size_t> >& section
) -> std::tuple<bool, std::list<std::shared_ptr<masm::elf::macro_subsection<address_size_t>>>> 
{
	using token_class_t = const std::set<masm::frontend::token_type>;
	static const token_class_t symbol = {masm::frontend::token_type::kSymbolDecl};
	static const token_class_t macro_invoke = {masm::frontend::token_type::kMacroInvoke};
	// TODO: Allow larger classes of macro arguments.
	static const token_class_t token_macro_args = {masm::frontend::token_type::kIdentifier};

	assert(section->body_raw);
	assert(section->body_token);

	bool error_resolving_tokens=false, error_counting_args=false;
	
	/**
	 *  Get the top level section which contains the current section.
	 */
	std::shared_ptr<masm::elf::top_level_section<address_size_t>> tls;
	if(auto as_macro = std::dynamic_pointer_cast<masm::elf::macro_subsection<address_size_t>>(section); as_macro != nullptr)  {
		tls = as_macro->containing_section.lock();
	}
	// Otherwise, we have a top level section, and it is the containing top level section.
	else if(tls = std::dynamic_pointer_cast<masm::elf::top_level_section<address_size_t>>(section); tls != nullptr) {}

	/**
	 * Recursively remove any leaf nodes in the graph, until no more nodes can be removed.
	 * If not all nodes can be removed, then there is a cycle in the graph, and compilation must fail.
	 * 
	 * Cycles are attributed to the parent, rather than the current section.
	 */
    auto out_graph = reduce(tls->invoke_dependency_graph);
	//std::cout << out_graph;
	// The graph had a cycle, must signal that assembly must be aborted.
    if(out_graph.num_nodes() != 0) {	
		auto as_macro = std::dynamic_pointer_cast<masm::elf::macro_subsection<address_size_t>>(section);

		// Determine which line of source code in the parent caused a cyclic include.
		auto parent = as_macro->direct_parent.lock();
		auto parent_macros = parent->body_preprocess.value().macro_line_mapping;
		auto parent_line = 0;
		for(auto [line, macro] : parent_macros) {
			if(macro == as_macro) parent_line = line;
		}

		project->message_resolver->log_message(parent, parent_line, {masm::message_type::kError, detail::error_circular_include});
		return {false, {}};
    }

	std::set<std::tuple<uint32_t, std::string, std::vector<std::string> > > potential_invocations;

	/**
	 * Extract macro definitions by evaluating tokens.
	 */
	for(auto [line_it, line] : section->body_token.value().tokens | boost::adaptors::indexed(0)) {
		auto first = line.begin(), last = line.end();

		// Parse (and ignore) a symbol declaration
		masm::frontend::match(first, last, symbol, true);

		// Check if this is a macro line.
		auto [matched, _1, macro_name] = masm::frontend::match(first, last, macro_invoke, true);
		if(!matched) continue;

		// Extract macro name
	
		std::vector<std::string> macro_args;

		// Check if there are macro args
		decltype(masm::frontend::match(first,last,{},{})) match_args;
		do {
			match_args = masm::frontend::match(first, last, token_macro_args, true);
			if(std::get<0>(match_args)) macro_args.emplace_back(std::get<2>(match_args));
			auto [had_comma, _2, _3] = masm::frontend::match(first, last, {masm::frontend::token_type::kComma}, true);
			// If no comma was present, then we can assume we are at the end of a macro list.
			if(!had_comma) break;
		} while(std::get<0>(match_args));

		// Consume comment if present
		masm::frontend::match(first, last, {masm::frontend::token_type::kComment}, true);
		// Check that we reached the end of the line
		auto [valid_invoke, _4, _5] = masm::frontend::match(first, last, {masm::frontend::token_type::kEmpty}, true);
		if(!valid_invoke) {
			project->message_resolver->log_message(section, line_it, {masm::message_type::kError, detail::error_invalid_macro});
			error_resolving_tokens = true;
		}

		potential_invocations.insert({line_it, macro_name, macro_args});

	}

	/**
	 * Create macro subsections and add links between sections.
	 */
	auto registry = project->macro_registry;
	std::list<std::shared_ptr<masm::elf::macro_subsection<address_size_t>>> children;
	for(auto [line, macro_name, macro_args] : potential_invocations) {
		if(!registry->contains(macro_name)) {
			error_counting_args = true;
			project->message_resolver->log_message(section, line, 
				{masm::message_type::kError, fmt::format(detail::error_does_not_exist, macro_name)}
			);
		}
		else if(auto macro_def = registry->macro(macro_name); macro_def->arg_count != macro_args.size()) {
			error_counting_args = true;
			project->message_resolver->log_message(section, line, 
				{masm::message_type::kError, fmt::format(detail::error_bad_arg_count, macro_args.size(), macro_def->arg_count)}
			);
		}
		else {
			// Delegate construction of macro to top-level section, which will mantain its own invariants.
			auto child_macro = tls->insert_macro(section, line, macro_name, macro_args, macro_def->macro_text);
			// Add child to top of priority queue.
			children.emplace_back(child_macro);
		}
	}


	return {!(error_resolving_tokens || error_counting_args), children};
}
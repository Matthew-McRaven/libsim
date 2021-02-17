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
	std::shared_ptr<masm::elf::code_section<address_size_t> >& section,
	const flags& flags) 
	-> void
{
	using token_class_t = const std::set<masm::frontend::token_type>;
	static const token_class_t symbol = {masm::frontend::token_type::kSymbolDecl};
	static const token_class_t macro_invoke = {masm::frontend::token_type::kMacroInvoke};
	// TODO: Allow larger classes of macro arguments.
	static const token_class_t token_macro_args = {masm::frontend::token_type::kIdentifier};

	assert(section->body_raw);
	assert(section->body_token);

	std::set<std::tuple<uint32_t, std::string, std::vector<std::string> > > potential_invocations;
	bool error_resolving_tokens = false;
	int line_it = -1;

	/**
	 * Extract macro definitions by evaluating tokens.
	 */
	for(auto line : section->body_token.value().tokens) {
		line_it++;
		auto first = line.begin(), last = line.end();

		// Parse (and ignore) a symbol declaration
		masm::frontend::match(first, last, symbol, true);

		// Check if this is a macro line.
		auto [matched, match] = masm::frontend::match(first, last, macro_invoke, true);
		if(!matched) continue;

		// Extract macro name
		auto [_1, macro_name] = match;
		std::vector<std::string> macro_args;

		// Check if there are macro args
		for(decltype(masm::frontend::match(first,last,{},{})) matched; matched.first; matched = masm::frontend::match(first, last, token_macro_args, true)) {
			macro_args.emplace_back(matched.second.second);
			auto [had_comma, _2] = masm::frontend::match(first, last, {masm::frontend::token_type::kComma}, true);
			// If no comma was present, then we can assume we are at the end of a macro list.
			if(!had_comma) break;
		}

		// Consume comment if present
		masm::frontend::match(first, last, {masm::frontend::token_type::kComment}, true);
		// Check that we reached the end of the line
		auto [valid_invoke, _3] = masm::frontend::match(first, last, {masm::frontend::token_type::kEmpty}, true);
		if(!valid_invoke) {
			project->message_resolver->log_message(section, line_it, {masm::message_type::kError, "Invalid macro definition"});
			error_resolving_tokens = true;
		}

		potential_invocations.insert({line_it, macro_name, macro_args});

	}

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
	 * Create macro subsections and add links between sections.
	 */
	auto registry = project->macro_registry;
	for(auto [line, macro_name, macro_args] : potential_invocations) {
		if(!registry->contains(macro_name)) {
			project->message_resolver->log_message(section, line, {masm::message_type::kError, ";ERROR: Referenced macro does not exist.."});
		}
		else if(auto macro_def = registry->macro(macro_name); macro_def->arg_count != macro_args.size()) {
			project->message_resolver->log_message(section, line, {masm::message_type::kError, ";ERROR: Macro supplied wrong number of arguments."});
		}
		else {
			// Delegate construction of macro to top-level section, which will mantain its own invariants.
			auto child_macro = tls->insert_macro(section, line, macro_name, macro_args, macro_def->macro_text);
			// Add child to top of priority queue.
			project->section_queue.push({masm::project::toolchain_stage::RAW, child_macro});
		}
	}

	/**
	 * Check for cyclic includes.
	 */
	// Recursively remove any leaf nodes in the graph,
    // until no more nodes can be removed.
    // If not all nodes can be removed, then there is a
    // cycle in the graph, and compilation must fail.
    auto out_graph = reduce(tls->macro_dependency_graph);
    std::stringstream str;
    str << out_graph;
	std::cout << out_graph;
	// The graph had a cycle, we need to signal that compilation must be aborted.
    if(out_graph.num_nodes() != 0) {
		// Must crawl tree to figure out which lines in the root instance
        // need error messages appended.
        for(auto searched_section : tls->macro_lookup | boost::adaptors::map_values) {
            if( out_graph.find(searched_section->header.index) != out_graph.end()) {
                auto path_to_cycle = NGraph::path(out_graph, tls->header.index, searched_section->header.index);

				// Find the section that included the "evil" macro who had a circular dependency.
				// That way we can look the source line in the including module which triggered the circularity.
				std::shared_ptr<masm::elf::code_section<address_size_t> > including_section;
				if(auto size = path_to_cycle.size(); size == 2) {
					including_section = tls;
				}
				else {
					auto begin = path_to_cycle.begin();
					// TODO: Probably off by one.
					std::advance(begin, size-1);
					including_section = tls->macro_lookup[*begin];
				}

				// Figure out which line of the last module caused the problem.
				auto line_number = 0;
				for(auto element : including_section->line_to_macro) {
					// Check if the current line's macro matches the macro which triggered the cycle.
					if(element.second->header.index == searched_section->header.index) {
						line_number = element.first;
						break;
					}
				}
				
				// Register an error using the section that had the evil include.
				auto as_code = std::static_pointer_cast<masm::elf::code_section<address_size_t>>(including_section);
				project->message_resolver->log_message(including_section, line_number, {masm::message_type::kError, ";ERROR: Circular macro inclusion detected."});
                break;
            }
        }
    }

}
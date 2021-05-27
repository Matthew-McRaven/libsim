#include "tokenizer.hpp"

#include <regex>
#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <fmt/core.h>

#include "tokens.hpp"
#include "masm/project/section.hpp"


template<typename address_size_t, typename tokenizer_t>
auto masm::frontend::tokenizer<address_size_t, tokenizer_t>::tokenize(
	std::shared_ptr<masm::project::project<address_size_t> >& project, 
	std::shared_ptr<masm::elf::code_section<address_size_t> >& section
) -> bool
{
	assert(section->body_raw);

	// Check if we are a macro subsection, and would therefore have macro arguments needing substitution.
	auto section_as_macro = dynamic_cast<masm::elf::macro_subsection<address_size_t>*>(section.get());
	bool perform_macro_substitution = (section_as_macro != nullptr);
	
	bool success = true;
	section->body_token = masm::elf::code::token();
	for(auto [it, line] : section->body_raw.value().lines | boost::adaptors::indexed(0)) {

		// Perform macro substitution on the current line if we are a macro.
		// We perform macro substitution as early as possible, so as to avoid additional complexity in later stages.
		// By performing replacement now, the preprocessor doesn't need to re-process lines with $macro substitutions.
		if(perform_macro_substitution) {
			for(auto [index, substitution] : section_as_macro->macro_args | boost::adaptors::indexed(0)) {
				line = std::regex_replace(line, std::regex(fmt::format("\\${}", index)), substitution);
			}
		}

		auto result = masm::frontend::tokenize(line, tokenizer_);
		// TODO: Handle tokenization errors.
		if(success &= result.success; !result.success) {
			masm::message err_msg {masm::message_type::kError, result.error_message.value()};
			project->message_resolver->log_message(section, it, err_msg);
			// Insert no tokens, because line has no meaning.
			section->body_token.value().tokens.push_back({{}});
			// Don't break. Finish iterating, so we can catch all tokenization errors in one sweep.
		}
		else {
			section->body_token.value().tokens.push_back(result.tokens);
		}
	}
	return success;
}

template<typename tokenizer_t>
masm::frontend::tokenize_result<typename tokenizer_t::token_type> masm::frontend::tokenize(std::string input, tokenizer_t& tokenizer)
{
	auto first = input.begin(), last=input.end();
	typename tokenizer_t::iterator_type iter = tokenizer.begin(first, last);
	typename tokenizer_t::iterator_type end = tokenizer.end();

	tokenize_result<typename tokenizer_t::token_type> ret;
	while (iter != end) {
		if(!token_is_valid(*iter)) {
			ret.success = false;
			ret.error_message = "Failed to parse token.";
			break;
		}

		std::string s;
		boost::spirit::traits::assign_to(*iter, s);
		auto get_token_type = tokenizer.convert_token_to_type(*iter);
		ret.tokens.emplace_back(std::pair<typename tokenizer_t::token_type, std::string>{get_token_type, s});
		iter++;
	}
	ret.rest = std::string(first, last);
	return ret;
}

template<typename token_enum_t>
std::tuple<bool, token_enum_t, std::string> masm::frontend::match(
    typename tokenized_line_t<token_enum_t>::iterator& first, const typename tokenized_line_t<token_enum_t>::iterator& end, 
    const std::set<masm::frontend::token_type> set, bool advance_iterator)
{
	std::tuple<bool, token_enum_t, std::string> ret = {false, {}, {}};
	if(first == end) return ret;
	else if(auto val = set.find(first->first); 
		val != set.end()) {
		ret = {true, first->first, first->second};
		if(advance_iterator) ++first;
		return ret;
	}
	else return ret;
}

template<typename token_enum_t>
std::tuple<bool, token_enum_t, std::string> masm::frontend:: match_not(
    typename tokenized_line_t<token_enum_t>::iterator& first, const typename tokenized_line_t<token_enum_t>::iterator& end, 
    const std::set<masm::frontend::token_type> set, bool advance_iterator ) 
{
	std::tuple<bool, token_enum_t, std::string> ret = {false, {}, {}};
	if(first == end) return ret;
	else if(auto val = set.find(first->first); 
		val == set.end()) {
		ret = {true, first->first, first->second};
		if(advance_iterator) ++first;
		return ret;
	}
	else return ret;
}
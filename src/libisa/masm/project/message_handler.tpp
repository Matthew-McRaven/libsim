#include "message_handler.hpp"
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
template <typename address_size_t>
void masm::message_handler<address_size_t>::log_message(std::shared_ptr<masm::elf::code_section<address_size_t> > section, 
	uint32_t line, message message)
{
	messages.emplace_front(message_t{section, line, message});
}

template <typename address_size_t>
std::list<std::tuple<uint32_t /*line*/, masm::message> > masm::message_handler<address_size_t>::errors_for_section(
	std::shared_ptr<masm::elf::code_section<address_size_t> >& section, bool recurse)
{
	using ret_t = std::tuple<uint32_t, masm::message>;
	// Can only recurse if preprocessor information has been filled in.
	if(recurse && section->body_preprocess) {
		std::list<ret_t> ret = errors_for_section(section, false);
		for(auto [line, macro]: section->body_preprocess->macro_line_mapping ) {
			auto as_code = std::static_pointer_cast<masm::elf::code_section<address_size_t>>(macro);
			auto local_errors = errors_for_section(as_code, true);
			for(auto &[local_line, local_message] : local_errors) local_line=line;
			ret.insert(ret.end(), local_errors.begin(), local_errors.end());
		}
		return ret;
	}
	else{
		auto errors = messages 
			| boost::adaptors::filtered([&section](auto line){return std::get<0>(line) == section;})
			| boost::adaptors::transformed([](auto& line){return ret_t{std::get<1>(line), std::get<2>(line)};});
		return std::list<ret_t>(errors.begin(), errors.end());
	}
}

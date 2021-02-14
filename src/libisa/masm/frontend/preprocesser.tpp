#include "preprocesser.hpp"
#include <boost/range/adaptor/map.hpp>

#include "tokens.hpp"

template<typename address_size_t>
masm::frontend::preprocessor<address_size_t>::~preprocessor() = default;

template<typename address_size_t>
auto masm::frontend::preprocessor<address_size_t>::preprocess(
	std::shared_ptr<masm::project::project<address_size_t> >& project, 
	const masm::frontend::preprocessor<address_size_t>::flags& flag) 
	-> void
{
	for(auto& image: project->images | boost::adaptors::map_values)
	{
		// Do work on image.
		this->recurse_image(project, image, flag);
	}
}

template<typename address_size_t>
auto masm::frontend::preprocessor<address_size_t>::recurse_image(
	std::shared_ptr<masm::project::project<address_size_t> >& project, 
	std::shared_ptr<masm::elf::image<address_size_t> >& image,
	const masm::frontend::preprocessor<address_size_t>::flags& flag) 
	-> void
{
	for(auto section: image->sections)
	{
		// Do work on section.
		// Re-cast pointer so it can deduce types correctly.
		auto v = std::shared_ptr<masm::elf::code_section<address_size_t> >(section);
		this->recurse_section(project, image, v, flag);
	}
}

template<typename address_size_t>
auto masm::frontend::preprocessor<address_size_t>::recurse_section(
	std::shared_ptr<masm::project::project<address_size_t> >& project, 
	std::shared_ptr<masm::elf::image<address_size_t> >& image,
	std::shared_ptr<masm::elf::code_section<address_size_t> >& section,
	const masm::frontend::preprocessor<address_size_t>::flags& flag) 
	-> void
{
	assert(section->body_raw);

	// Set up our line parser.
	using token_t = lex::lexertl::token<std::string::iterator, boost::mpl::vector<long long, std::string>>;
	using lexer_t = lex::lexertl::actor_lexer<token_t>;
	masm::frontend::tokenizer<lexer_t> lexer;

	int it = 0;

	for(auto line : section->body_raw.value().lines) {
		// Parse only macro lines!!!
		std::cout << line << std::endl;
		auto head = line.begin();
		lexer_t::iterator_type iter = lexer.begin(head, line.end());
    	lexer_t::iterator_type end = lexer.end();
		while (iter != end) {
			std::string s;
			boost::spirit::traits::assign_to(*iter, s);
			std::cout << s << std::endl;
			break;
		}
	}
}
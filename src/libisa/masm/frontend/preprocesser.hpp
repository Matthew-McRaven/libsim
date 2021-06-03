
#pragma once

#include <tuple>
#include <string>

#include "masm/project/project.hpp"
#include "masm/project/image.hpp"
namespace masm::frontend
{
template<typename address_size_t, typename tokenizer_t>
class preprocessor
{
public:
    struct flags{};
    preprocessor(){}
    preprocessor(const flags& flag){}
    virtual ~preprocessor();
    virtual auto preprocess(
        std::shared_ptr<masm::project::project<address_size_t> >& project, 
        std::shared_ptr<masm::elf::code_section<address_size_t> >& section
    ) -> std::tuple<bool, std::list<std::shared_ptr<masm::elf::macro_subsection<address_size_t>>>>;

protected:
    
    tokenizer_t tokenizer;
};

} // End namespace masm::frontend
namespace masm::frontend::detail {
    static const std::string error_does_not_exist = ";ERROR: Invoked macro '{}' does not exist.";
    static const std::string error_bad_arg_count = ";ERROR: Macro invoked with {} arguments, expected {}.";
    static const std::string error_circular_include = ";ERROR: Circular macro inclusion detected.";
    static const std::string error_invalid_macro = ";ERROR: Unparsable macro invocation.";
} // End namespace masm::frontend::detail

#include "masm/frontend/preprocesser.tpp"
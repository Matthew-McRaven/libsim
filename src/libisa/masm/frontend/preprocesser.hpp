
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
    struct result {};
    struct flags{};
    virtual ~preprocessor();
    virtual auto preprocess(
        std::shared_ptr<masm::project::project<address_size_t> >& project, 
        std::shared_ptr<masm::elf::code_section<address_size_t> >& section,
        const flags& flag) -> void;

protected:
    
    tokenizer_t tokenizer;
};

} // End namespace masm::frontend
namespace masm::frontend::detail {
    static const std::string tooManyMacros = ";ERROR: Only one macro may be referenced per line.";
    static const std::string noIdentifier = ";ERROR: A @ must be followed by a string identifier.";
    static const std::string noSuchMaro = ";ERROR: Referenced macro does not exist.";
    static const std::string badArgCount = ";ERROR: Macro supplied wrong number of arguments.";
    static const std::string noDollarInMacro = ";ERROR: Cannot use $ as part of a macro identifier.";
    static const std::string invalidArg = ";ERROR: Bad argument: {}. Cannot use $ in macro argument.";
    static const std::string circularInclude = ";ERROR: Circular macro inclusion detected.";
    static const std::string selfRefence = ";ERROR: Macro definition invokes itself.";
} // End namespace masm::frontend::detail

#include "masm/frontend/preprocesser.tpp"
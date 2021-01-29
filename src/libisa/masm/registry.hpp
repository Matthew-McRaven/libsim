#pragma once

#include <map>
#include <memory>
#include <string>
#include <tuple>

#include <range/v3/all.hpp>

#include "macro.hpp"

namespace masm {
/*
 * This object keeps track of which macros exist, how many arguments they take, and which
 * level in the macro heirarchy they occupy.
 *
 * Only one macro may exist with the same name - there cannot be multiple macros with the same name
 * and different levels and/or different argument counts.
 */
class macro_registry
{
    auto filter_macros(MacroType type) const -> std::vector<std::shared_ptr<const Macro>>;;
public:
    macro_registry();
    ~macro_registry() = default;
    bool contains(const std::string& macro_name) const;
    std::shared_ptr<const Macro> macro(const std::string& macro_name) const;

    auto core_macros() const -> decltype(filter_macros({}));
    auto system_macros() const -> decltype(filter_macros({}));
    auto custom_macros() const -> decltype(filter_macros({}));

    // Register a macro to the specified level with a given argument count.
    bool register_unary_system_call(const std::string& macro_name, const std::string& template_text);
    bool register_nonunary_system_call(const std::string& macro_name, const std::string& template_text);
    
    void clear_macros(const MacroType type);

    bool register_macro(const std::string& macro_name, const std::string& macro_text, const MacroType type);
private:
    auto filter_macros(MacroType type) -> std::vector<std::shared_ptr<const Macro>>;
    
    std::map<std::string, std::shared_ptr<Macro>> _registry;

};
}; // End namespace masm.
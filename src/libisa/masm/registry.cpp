#include "registry.hpp"

#include <iostream>
#include <algorithm>
#include <fmt/core.h>
#include <boost/range/adaptor/map.hpp>

#include "macroparser.hpp"

auto masm::macro_registry::filter_macros(MacroType type) const -> decltype(macro_registry::filter_macros({}))
{
	// TODO: Return a view instead of a container. This will improve memory efficiency
	std::vector<std::shared_ptr<const Macro> > ret;
	for(auto const &i : _registry | boost::adaptors::map_values)
	{
		ret.emplace_back(i);
	}
	return ret;
}

masm::macro_registry::macro_registry() : _registry()
{

}

bool masm::macro_registry::contains(const std::string& macro_name) const
{
	auto v = _registry.find(macro_name);
	return v != _registry.end();
}

std::shared_ptr<const masm::Macro> masm::macro_registry::macro(const std::string& macro_name) const
{
	if(auto v = _registry.find(macro_name); v != _registry.end()) return v->second;
	return nullptr;
}

auto masm::macro_registry::core_macros() const -> decltype(macro_registry::core_macros())
{
	return filter_macros(MacroType::CoreMacro);
}

auto masm::macro_registry::system_macros() const -> decltype(macro_registry::system_macros())
{
	return filter_macros(MacroType::SystemMacro);
}

auto masm::macro_registry::custom_macros() const -> decltype(macro_registry::custom_macros())
{
	return filter_macros(MacroType::UserMacro);
}

bool masm::macro_registry::register_unary_system_call(const std::string& macro_name, const std::string& template_text)
{
	std::string macro_text = fmt::format(template_text, macro_name);
	return register_macro(macro_name, macro_text, MacroType::SystemMacro);
}
bool masm::macro_registry::register_nonunary_system_call(const std::string& macro_name, const std::string& template_text)
{
	std::string macro_text = fmt::format(template_text, macro_name);
	return register_macro(macro_name, macro_text, MacroType::SystemMacro);
}

bool masm::macro_registry::register_macro(const std::string& macro_name, const std::string& macro_text, const MacroType type)
{
	if(_registry.find(macro_name) != _registry.end()) {
        return false;
    }
    auto [success, name, arg_count] = analyze_macro_definition(macro_text);
    if(!success) {
        return false;
    }
    if(macro_name != name) {
        return false;
    }

    std::shared_ptr<Macro> new_macro = std::make_shared<Macro>();
    new_macro->macro_name = macro_name;
    new_macro->macro_text = macro_text;
    new_macro->arg_count = arg_count;
    new_macro->type = type;

	// Erase the macro definition from the text, since we have the meta-information to recreate it.
	// Keeping this line complicates the preprocessor.
	new_macro->macro_text.erase(0, macro_text.find("\n")+1);

    _registry[macro_name] = new_macro ;
    return true;
}

void masm::macro_registry::clear_macros(const MacroType type)
{
	// Don't catch return value, it makes emcmake mad.
	std::erase_if(_registry, [&type](const auto& item) {
		auto const& [key, value] = item;
		return value->type == type;
	});;
}
#include "registry.hpp"

#include <algorithm>
#include <fmt/core.h>

#include "macroparser.hpp"

auto masm::macro_registry::filter_macros(MacroType type) const -> decltype(macro_registry::filter_macros({}))
{
	auto filter = [&type](auto ref){return ref.second->type == type;};
	static auto transform = [](auto ref){return ref.second;};
	auto view =  _registry 
		| ranges::views::filter(filter) 
		| ranges::views::transform(transform);
	// TODO: Return a view instead of a container. This will improve memory efficiency
	return view | ranges::to<std::vector<std::shared_ptr<const Macro> > >;
}

masm::macro_registry::macro_registry(const std::string& registry_directory) : _registry()
{
    //QString builtinDir = ":/help-asm/macros/builtin";
    //registerCoreMacros(QDir(builtinDir));
    //nonunaryMacroTemplate = Pep::resToString(":/help-asm/macros/systemcall/SCALL.txt", false);
    //unaryMacroTemplate = Pep::resToString(":/help-asm/macros/systemcall/USCALL.txt", false);
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
	if(_registry.find(macro_name) == _registry.end()) {
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
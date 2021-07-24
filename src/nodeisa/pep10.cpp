#include "pep10.hpp"

#include "asmb/pep10/highlight.hpp"

#include <iostream>
#include <emscripten.h>

pep10::wrapped_isa_definition::wrapped_isa_definition()
{
    
}

EMSCRIPTEN_BINDINGS(pep10) {

  auto im = emscripten::enum_<isa::pep10::instruction_mnemonic>("InstructionMnemonic");
  for(auto [enu, str] : magic_enum::enum_entries<isa::pep10::instruction_mnemonic>()) {
    im.value(str.data(), enu);
  }

  auto am =emscripten::enum_<isa::pep10::addressing_mode>("AddressingMode");
  for(auto [enu, str] : magic_enum::enum_entries<isa::pep10::addressing_mode>()) {
    am.value(str.data(), enu);
  }

  auto ac = emscripten::enum_<isa::pep10::addressing_class>("AddressingClass");
  for(auto [enu, str] : magic_enum::enum_entries<isa::pep10::addressing_class>()) {
    ac.value(str.data(), enu);
  }

  auto arr = emscripten::value_array<std::array<bool, magic_enum::enum_count<isa::pep10::CSR>()>>("CSRModified");
  loop<decltype(arr),magic_enum::enum_count<isa::pep10::CSR>()-1>().fn(arr);


  auto csr = emscripten::enum_<isa::pep10::CSR>("CSR");
  for(auto [enu, str] : magic_enum::enum_entries<isa::pep10::CSR>()) {
    csr.value(str.data(), enu);
  }

  emscripten::value_object<isa::pep10::instruction_definition>("InstructionDefinition")
    .field("bit_pattern", &isa::pep10::instruction_definition::bit_pattern)
    .field("iformat", &isa::pep10::instruction_definition::iformat)
    .field("csr_modified", &isa::pep10::instruction_definition::CSR_modified)
    .field("mnemonic", &isa::pep10::instruction_definition::mnemonic)
    .field("is_unary", &isa::pep10::instruction_definition::is_unary)
    .field("comment", &isa::pep10::instruction_definition::comment);

  emscripten::register_map<isa::pep10::instruction_mnemonic, std::shared_ptr<isa::pep10::instruction_definition > >("ISAMap");
  emscripten::class_<isa::pep10::addr_map>("amap");

  using instr_def_t = isa::pep10::instruction_definition;
  emscripten::class_<std::shared_ptr<instr_def_t>>("Smarty")
    // Must manually tell each property what it's type is, because it can't infer it from
    // the getter. Failing to specify property type will cause it to explode.
    .property<decltype(instr_def_t::bit_pattern)>("bit_pattern", &bit_pattern<instr_def_t>)
    .property<decltype(instr_def_t::iformat)>("iformat", &iformat<instr_def_t>)
    .property<decltype(instr_def_t::CSR_modified)>("CSR_modified", &CSR_modified<instr_def_t>)
    .property<decltype(instr_def_t::mnemonic)>("mnemonic", &mnemonic<instr_def_t>)
    .property<decltype(instr_def_t::is_unary)>("is_unary", &is_unary<instr_def_t>);

  auto oe = emscripten::value_array<std::array<isa::pep10::addr_map, 255> >("OderEyes");
  loop<decltype(oe), 255>().fn(oe);
  emscripten::class_<pep10::wrapped_isa_definition>("IsaDefinition")
    .constructor()
    .property("isa", &pep10::wrapped_isa_definition::get_isa)
    .property("map", &pep10::wrapped_isa_definition::get_map);

  auto highlight_enum = emscripten::enum_<asmb::highlight_type>("highlight_enum");
  for(auto [enu, str] : magic_enum::enum_entries<asmb::highlight_type>()) {
    highlight_enum.value(str.data(), enu);
  };
  emscripten::register_map<asmb::highlight_type, std::string>("HighlightRulesMap");
  emscripten::function("highlight_rules", &asmb::pep10::construct_rules);
}
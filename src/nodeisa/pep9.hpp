#pragma once

#include <emscripten/html5.h>
#include <emscripten/bind.h>

#include "magic_enum.hpp"

#include "isa/pep9/instruction.hpp"
#include <iostream>

#include "helper.hpp"

// TODO: Move bindings & definitions to CPP file.

namespace pep9 {
struct wrapped_isa_definition
{
  wrapped_isa_definition();
  auto get_isa() const -> decltype(isa::pep9::isa_definition::get_definition().isa)
  {
  
    return isa::pep9::isa_definition::get_definition().isa;
  }
  auto get_map() const -> decltype(isa::pep9::isa_definition::get_definition().riproll)
  {
    return isa::pep9::isa_definition::get_definition().riproll;
  }
private:
  isa::pep9::isa_definition def;
};



EMSCRIPTEN_BINDINGS(pep9) {

  auto im = emscripten::enum_<isa::pep9::instruction_mnemonic>("InstructionMnemonic");
  for(auto [enu, str] : magic_enum::enum_entries<isa::pep9::instruction_mnemonic>()) {
    im.value(str.data(), enu);
  }

  auto am =emscripten::enum_<isa::pep9::addressing_mode>("AddressingMode");
  for(auto [enu, str] : magic_enum::enum_entries<isa::pep9::addressing_mode>()) {
    am.value(str.data(), enu);
  }

  auto ac = emscripten::enum_<isa::pep9::addressing_class>("AddressingClass");
  for(auto [enu, str] : magic_enum::enum_entries<isa::pep9::addressing_class>()) {
    ac.value(str.data(), enu);
  }

  auto arr = emscripten::value_array<std::array<bool, magic_enum::enum_count<isa::pep9::CSR>()>>("CSRModified");
  loop<decltype(arr),magic_enum::enum_count<isa::pep9::CSR>()-1>().fn(arr);


  auto csr = emscripten::enum_<isa::pep9::CSR>("CSR");
  for(auto [enu, str] : magic_enum::enum_entries<isa::pep9::CSR>()) {
    csr.value(str.data(), enu);
  }

  emscripten::value_object<isa::pep9::instruction_definition<uint8_t>>("InstructionDefinition")
    .field("bit_pattern", &isa::pep9::instruction_definition<uint8_t>::bit_pattern)
    .field("iformat", &isa::pep9::instruction_definition<uint8_t>::iformat)
    .field("csr_modified", &isa::pep9::instruction_definition<uint8_t>::CSR_modified)
    .field("mnemonic", &isa::pep9::instruction_definition<uint8_t>::mnemonic)
    .field("is_unary", &isa::pep9::instruction_definition<uint8_t>::is_unary)
    .field("comment", &isa::pep9::instruction_definition<uint8_t>::comment);


  emscripten::register_map<isa::pep9::instruction_mnemonic, std::shared_ptr<isa::pep9::instruction_definition<uint8_t> > >("ISAMap");
  
  emscripten::class_<isa::pep9::addr_map>("amap");

  using instr_def_t = isa::pep9::instruction_definition<uint8_t>;
  emscripten::class_<std::shared_ptr<instr_def_t>>("Smarty")
    // Must manually tell each property what it's type is, because it can't infer it from
    // the getter. Failing to specify property type will cause it to explode.
    .property<decltype(instr_def_t::bit_pattern)>("bit_pattern", &bit_pattern<instr_def_t>)
    .property<decltype(instr_def_t::iformat)>("iformat", &iformat<instr_def_t>)
    .property<decltype(instr_def_t::CSR_modified)>("CSR_modified", &CSR_modified<instr_def_t>)
    .property<decltype(instr_def_t::mnemonic)>("mnemonic", &mnemonic<instr_def_t>)
    .property<decltype(instr_def_t::is_unary)>("is_unary", &is_unary<instr_def_t>);

  auto oe = emscripten::value_array<std::array<isa::pep9::addr_map, 255> >("OderEyes");
  loop<decltype(oe), 255>().fn(oe);
  emscripten::class_<wrapped_isa_definition>("IsaDefinition")
    .constructor()
    .property("isa", &wrapped_isa_definition::get_isa)
    .property("map", &wrapped_isa_definition::get_map);
}

}; //End namespace pep9
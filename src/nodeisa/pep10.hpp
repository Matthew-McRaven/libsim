#pragma once

#include <emscripten/html5.h>
#include <emscripten/bind.h>

#include "magic_enum.hpp"

#include "isa/pep10/instruction.hpp"
#include <iostream>

#include "helper.hpp"
struct wrapped_isa_definition
{
  wrapped_isa_definition();
  const std::map<isa::pep10::instruction_mnemonic,isa::pep10::instruction_definition<uint8_t> > &get_isa() const
  {
  
    return isa::pep10::isa_definition::get_definition().isa;
  }
  const std::array<isa::pep10::addr_map, 256>& get_map() const
  {
    return isa::pep10::isa_definition::get_definition().riproll;
  }
private:
  isa::pep10::isa_definition def;
};

struct wrapped_instruction_definition
{
  wrapped_instruction_definition(){}
};


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

  auto arr = emscripten::value_array<std::array<bool, int(isa::pep10::CSR::MAX)>>("CSRModified");
  loop<decltype(arr), 4 - 1>().fn(arr);


  auto csr = emscripten::enum_<isa::pep10::CSR>("CSR");
  for(auto [enu, str] : magic_enum::enum_entries<isa::pep10::CSR>()) {
    csr.value(str.data(), enu);
  }

  emscripten::value_object<isa::pep10::instruction_definition<uint8_t>>("InstructionDefinition")
    .field("bit_pattern", &isa::pep10::instruction_definition<uint8_t>::bit_pattern)
    .field("iformat", &isa::pep10::instruction_definition<uint8_t>::iformat)
    .field("csr_modified", &isa::pep10::instruction_definition<uint8_t>::CSR_modified)
    .field("mnemonic", &isa::pep10::instruction_definition<uint8_t>::mnemonic)
    .field("is_unary", &isa::pep10::instruction_definition<uint8_t>::is_unary)
    .field("comment", &isa::pep10::instruction_definition<uint8_t>::comment);

  emscripten::register_map<isa::pep10::instruction_mnemonic, isa::pep10::instruction_definition<uint8_t>>("ISAMap");

 
  auto oe = emscripten::value_array<std::array<isa::pep10::addr_map, 255> >("OderEyes");
  loop<decltype(oe), 256-1>().fn(oe);
  emscripten::class_<wrapped_isa_definition>("IsaDefinition")
    .constructor()
    .property("isa", &wrapped_isa_definition::get_isa)
    .property("map", &wrapped_isa_definition::get_map);
}


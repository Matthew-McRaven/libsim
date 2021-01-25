#pragma once

#include <emscripten/html5.h>
#include <emscripten/bind.h>

#include "magic_enum.hpp"

#include "isa/pep9/instruction.hpp"
#include <iostream>

#include "helper.hpp"
namespace pep9 {
struct wrapped_isa_definition
{
  wrapped_isa_definition();
  const isa::pep9::instruction_definition<uint8_t>& get_mnemonic(isa::pep9::instruction_mnemonic mn)
  {
    return isa::pep9::isa_definition::get_definition().isa[static_cast<int>(mn)];
  }
  const std::array<isa::pep9::instruction_definition<uint8_t>, (int) isa::pep9::instruction_mnemonic::MAX> &get_isa() const
  {
  
    return isa::pep9::isa_definition::get_definition().isa;
  }
  const std::array<isa::pep9::addr_map, 256>& get_map() const
  {
    return isa::pep9::isa_definition::get_definition().riproll;
  }
private:
  isa::pep9::isa_definition def;
};

struct wrapped_instruction_definition
{
  wrapped_instruction_definition(){}
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

  auto arr = emscripten::value_array<std::array<bool, int(isa::pep9::CSR::MAX)>>("CSRModified");
  loop<decltype(arr), 4 - 1>().fn(arr);


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


  auto we = emscripten::value_array<std::array<isa::pep9::instruction_definition<uint8_t>, (int) isa::pep9::instruction_mnemonic::MAX>>("WildEyes");
  loop<decltype(we), int (isa::pep9::instruction_mnemonic::MAX)-1>().fn(we);
  emscripten::value_object<isa::pep9::addr_map>("MeThinks")
    .field("inst", &isa::pep9::addr_map::inst)
    .field("addr", &isa::pep9::addr_map::addr);
    
  auto oe = emscripten::value_array<std::array<isa::pep9::addr_map, 255> >("OderEyes");
  loop<decltype(oe), 256-1>().fn(oe);
  emscripten::class_<wrapped_isa_definition>("IsaDefinition")
    .constructor()
    .property("isa", &wrapped_isa_definition::get_isa)
    .function("get_instruction", &wrapped_isa_definition::get_mnemonic)
    .property("map", &wrapped_isa_definition::get_map);
}

}; //End namespace pep9
#pragma once

#include <emscripten/html5.h>
#include <emscripten/bind.h>

#include "magic_enum.hpp"

#include "isa/pep10/instruction.hpp"
#include <iostream>

#include "helper.hpp"
namespace pep10 {

struct wrapped_isa_definition
{
  wrapped_isa_definition();
  auto get_isa() const -> decltype(isa::pep10::isa_definition::get_definition().isa)
  {
  
    return isa::pep10::isa_definition::get_definition().isa;
  }
  auto get_map() const -> decltype(isa::pep10::isa_definition::get_definition().riproll)
  {
    return isa::pep10::isa_definition::get_definition().riproll;
  }
private:
  isa::pep10::isa_definition def;
};

}; // End namespace pep10




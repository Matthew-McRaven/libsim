#pragma once

#include <emscripten/html5.h>
#include <emscripten/bind.h>

#include "magic_enum.hpp"

#include "isa/pep9/pep9.hpp"
#include <iostream>

#include "helper.hpp"

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

}; //End namespace pep9
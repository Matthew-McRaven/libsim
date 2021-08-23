#pragma once
// Taken from:
// https://github.com/ned14/status-code
#include <initializer_list>

#include <outcome.hpp>

// This is the custom error code enum
enum class PepElfErrc 
{
  Success     = 0, // 0 should not represent an error.
  NoOSText, //There was no os.text section. This is unrecoverable.
  NoOSMMIO, // No os.mmio section. This is unrecoverable.
  NoOSSymtab, // No os.symtab section. This is unrecoverable.
  BadMMIOSymbol, // os.mmio entry did not point to a valid symbol. This is not recoverable.
  InvalidMMIOType,
  
};

// To synthesise a custom status code domain for `PepElfErrc`, inject the following
// template specialisation:
SYSTEM_ERROR2_NAMESPACE_BEGIN
template <>
struct quick_status_code_from_enum<PepElfErrc>
  : quick_status_code_from_enum_defaults<PepElfErrc>
{
  // Text name of the enum
  static constexpr const auto domain_name = "Pep Elf Parsing Error";

  // Unique UUID for the enum. PLEASE use https://www.random.org/cgi-bin/randbyte?nbytes=16&format=h
  // Check: I generated a unique UUID-v4 from the random.org website and then converted it to a hex string.
  static constexpr const auto domain_uuid = "{d986324f-5ad7-426d-a978-4d1c5005dd3c}";

  // Map of each enum value to its text string, and list of semantically equivalent errc's
  static const std::initializer_list<mapping> &value_mappings()
  {
    static const std::initializer_list<mapping> v = {
    // Format is: { enum value, "string representation", { list of errc mappings ... } }
    {PepElfErrc::Success, "Elf image->Pep machine conversion succesful", {errc::success}},
    {PepElfErrc::NoOSText, "No os.text section was present in the provided image", {errc::invalid_argument}},
    {PepElfErrc::NoOSMMIO, "No os.mmio section was present in the provided image", {errc::invalid_argument}},
    {PepElfErrc::NoOSSymtab, "No os.symtab section was present in the provided image", {errc::invalid_argument}},
    {PepElfErrc::BadMMIOSymbol, "MMIO symbol_table_offset out of range", {errc::invalid_argument}},
    };
    return v;
  }
};
SYSTEM_ERROR2_NAMESPACE_END

// Kind helper method to convert enum to type-erased system code.
SYSTEM_ERROR2_NAMESPACE::system_code status_code(PepElfErrc c);// { return c; }
#pragma once
// Taken from:
// https://github.com/ned14/status-code
#include <initializer_list>

#include <outcome.hpp>

// Errors occuring reading / writing MMIO defs in an ELF file.
enum class ElfMMIOErrc
{
  Success     = 0, // 0 should not represent an error
  BadIOType, // The type of an MMIO entry was not kInput of kOutput. Not recoverable.
  BadByteCount, // Byte array must be a multiple of 4, but it was not. Not recoverable.
  NoSuchIndex, // Symbol index does not refer to a symbol in the os symtab. Not recoverable

};

// To synthesise a custom status code domain for `ElfMMIOErrc`, inject the following
// template specialisation:
SYSTEM_ERROR2_NAMESPACE_BEGIN
template <>
struct quick_status_code_from_enum<ElfMMIOErrc>
  : quick_status_code_from_enum_defaults<ElfMMIOErrc>
{
  // Text name of the enum
  static constexpr const auto domain_name = "ELF MMIO Configuration Error";

  // Unique UUID for the enum. PLEASE use https://www.random.org/cgi-bin/randbyte?nbytes=16&format=h
  // Check: I generated a unique UUID-v4 from the random.org website and then converted it to a hex string.
  static constexpr const auto domain_uuid = "{25ae3b0f-6b49-4c5b-af01-5af90c53616a}";

  // Map of each enum value to its text string, and list of semantically equivalent errc's
  static const std::initializer_list<mapping> &value_mappings()
  {
    static const std::initializer_list<mapping> v = {
    // Format is: { enum value, "string representation", { list of errc mappings ... } }
    {ElfMMIOErrc::Success, "MMIO configuration successful", {errc::success}},
    {ElfMMIOErrc::BadIOType, "MMIO Definition contained illegal type not in: {kInput, kOutput}", {errc::invalid_argument}},
    {ElfMMIOErrc::BadByteCount, "Serialized definition array must be a multiple of 4 bytes.", {errc::illegal_byte_sequence}},
    {ElfMMIOErrc::NoSuchIndex, "MMI Definition symtab index does not refer to a valid symbol.", {errc::invalid_argument}},
    };
    return v;
  }
};
SYSTEM_ERROR2_NAMESPACE_END

// Kind helper method to convert enum to type-erased system code.
SYSTEM_ERROR2_NAMESPACE::system_code status_code(ElfMMIOErrc c);// { return c; }
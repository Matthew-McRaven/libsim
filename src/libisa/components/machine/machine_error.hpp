#pragma once
// Taken from:
// https://github.com/ned14/status-code
#include <initializer_list>

#include <outcome.hpp>

// This is the custom error code enum
enum class MachineErrc
{
  Success     = 0, // 0 should not represent an error.
  NoSuchDevice // There is no memory mapped device with a given name. It is up to the user to determine if this is recoverable.
  // For example, lack of a power off port is probably recoverable, but lacking charOut may be recoverable.
};

// To synthesise a custom status code domain for `MachineErrc`, inject the following
// template specialisation:
SYSTEM_ERROR2_NAMESPACE_BEGIN
template <>
struct quick_status_code_from_enum<MachineErrc>
  : quick_status_code_from_enum_defaults<MachineErrc>
{
  // Text name of the enum
  static constexpr const auto domain_name = "Virtual Machine Error";

  // Unique UUID for the enum. PLEASE use https://www.random.org/cgi-bin/randbyte?nbytes=16&format=h
  // Check: I generated a unique UUID-v4 from the random.org website and then converted it to a hex string.
  static constexpr const auto domain_uuid = "{b38991ef-93ac-4d7f-95da-767fa58ed529}";

  // Map of each enum value to its text string, and list of semantically equivalent errc's
  static const std::initializer_list<mapping> &value_mappings()
  {
    static const std::initializer_list<mapping> v = {
    // Format is: { enum value, "string representation", { list of errc mappings ... } }
    {MachineErrc::Success, "Virtual machine operation succesful", {errc::success}},
    {MachineErrc::NoSuchDevice, "No MMIO device was found with the given name", {errc::invalid_argument}},
    };
    return v;
  }
};
SYSTEM_ERROR2_NAMESPACE_END

// Kind helper method to convert enum to type-erased system code.
SYSTEM_ERROR2_NAMESPACE::system_code status_code(MachineErrc c);// { return c; }
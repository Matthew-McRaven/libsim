#pragma once
// Taken from:
// https://github.com/ned14/status-code
#include <initializer_list>

#include <outcome.hpp>

// This is the custom error code enum
enum class ProcessorErrc
{
  Success     = 0, // 0 should not represent an error
  IllegalUnaryInstruction, // Attempted to decode an opcode as a unary instruction and failed. This is not recoverable.
  IllegalNonunaryInstruction, // Attempted to decode an opcode as a nonunary instruction and failed. This is not recoverable.
  IllegalAddressingMode, // Attempted to execute an instruction with an invalid addressing mode. Must not crash, but should terminate simulation.
};

// To synthesise a custom status code domain for `ProcessorErrc`, inject the following
// template specialisation:
SYSTEM_ERROR2_NAMESPACE_BEGIN
template <>
struct quick_status_code_from_enum<ProcessorErrc>
  : quick_status_code_from_enum_defaults<ProcessorErrc>
{
  // Text name of the enum
  static constexpr const auto domain_name = "Processor Error";

  // Unique UUID for the enum. PLEASE use https://www.random.org/cgi-bin/randbyte?nbytes=16&format=h
  // Check: I generated a unique UUID-v4 from the random.org website and then converted it to a hex string.
  static constexpr const auto domain_uuid = "{9e475aa4-9b6e-44cb-b175-6c912b608516}";

  // Map of each enum value to its text string, and list of semantically equivalent errc's
  static const std::initializer_list<mapping> &value_mappings()
  {
    static const std::initializer_list<mapping> v = {
    // Format is: { enum value, "string representation", { list of errc mappings ... } }
    {ProcessorErrc::Success, "Processor operation succesful", {errc::success}},
    {ProcessorErrc::IllegalUnaryInstruction, "Failed to decode unary instruction", {errc::not_supported}},
    {ProcessorErrc::IllegalNonunaryInstruction, "Failed to decode nonunary instruction", {errc::not_supported}},
    {ProcessorErrc::IllegalAddressingMode, "Illegal addressing mode", {errc::not_supported}},
    };
    return v;
  }
};
SYSTEM_ERROR2_NAMESPACE_END

// Kind helper method to convert enum to type-erased system code.
SYSTEM_ERROR2_NAMESPACE::system_code status_code(ProcessorErrc c);// { return c; }
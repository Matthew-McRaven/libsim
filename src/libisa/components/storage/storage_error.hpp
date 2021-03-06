#pragma once
// Taken from:
// https://github.com/ned14/status-code
#include <initializer_list>

#include <outcome.hpp>

// This is the custom error code enum
enum class StorageErrc
{
  Success     = 0, // 0 should not represent an error
  NoMMInput, // Storage operation failed because there was no memory-mapped input available. This is recoverable.
  ResizeError, // Attempted to resize a component which may not be resized. This is recoverable by creating additional instances of the class.
  Unwritable, // Write failed because the device doesn't support writing. The value written was ignored. This is recoverable.
  NoAvailableDelta, // Attempted to read a storage device whose history has been exhausted. This is recoverable.
  DeltaDisabled, // Attempted to read a storage device which has history disabled. This is recoverable.
  OOBRead, // Attempted to read beyond the end of the storage. This is not recoverable.
  OOBWrite, // Attempted to write beyond the end of the storage. This is not recoverable.
  IllegalWrite, // Attempting to write to a read-only address. This is not recoverable.
  IllegalInsert, // Inserting a storage into a layer failed because an offset is not contained within 2<<sizeof(offset_t). This is not recoverable.
  ResizeTooSmall, // Resizing a layered device would cause a chip to have unreachable offsets. This is not recoverable.
  NoSuchDevice, // When searching for a sub-device, the pointer passed was not a member of the layered storage. This is not recoverable.
};

// To synthesise a custom status code domain for `StorageErrc`, inject the following
// template specialisation:
SYSTEM_ERROR2_NAMESPACE_BEGIN
template <>
struct quick_status_code_from_enum<StorageErrc>
  : quick_status_code_from_enum_defaults<StorageErrc>
{
  // Text name of the enum
  static constexpr const auto domain_name = "Storage Error";

  // Unique UUID for the enum. PLEASE use https://www.random.org/cgi-bin/randbyte?nbytes=16&format=h
  // Check: I generated a unique UUID-v4 from the random.org website and then converted it to a hex string.
  static constexpr const auto domain_uuid = "{40c3de8a-3448-4b5b-8f94-a380b813d223}";

  // Map of each enum value to its text string, and list of semantically equivalent errc's
  static const std::initializer_list<mapping> &value_mappings()
  {
    static const std::initializer_list<mapping> v = {
    // Format is: { enum value, "string representation", { list of errc mappings ... } }
    {StorageErrc::Success, "Storage access successful", {errc::success}},
    {StorageErrc::NoMMInput, "No memory-mapped input available", {errc::io_error}},
    {StorageErrc::ResizeError, "Storage resize failed", {errc::not_enough_memory}},
    {StorageErrc::Unwritable, "Storage cannot be written to", {errc::not_supported}},
    {StorageErrc::NoAvailableDelta, "No delta is stored in this class", {errc::no_message}},
    {StorageErrc::DeltaDisabled, "Instance has deltas disabled", {errc::not_supported}},
    {StorageErrc::OOBRead, "Attempted to read beyond the end of the storage", {errc::bad_address}},
    {StorageErrc::OOBWrite, "Attempted to write beyond the end of the storage", {errc::bad_address}},
    {StorageErrc::IllegalWrite, "Attempted to write to a read-only storage device", {errc::invalid_argument}},
    {StorageErrc::IllegalInsert, "Insert into layered device would cause unreachable offsets", {errc::invalid_argument}},
    {StorageErrc::ResizeTooSmall, "Resizing layered device would create unreachable offsets", {errc::invalid_argument}},
    {StorageErrc::NoSuchDevice, "Could not find passed device in container", {errc::invalid_argument}},
    };
    return v;
  }
};
SYSTEM_ERROR2_NAMESPACE_END

// Kind helper method to convert enum to type-erased system code.
SYSTEM_ERROR2_NAMESPACE::system_code status_code(StorageErrc c);// { return c; }
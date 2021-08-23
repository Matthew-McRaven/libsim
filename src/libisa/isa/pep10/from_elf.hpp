#pragma once

#include <memory>

#include <elfio/elfio.hpp>
#include <outcome.hpp>

#include "isa/pep10/local_machine.hpp"
#include "isa/pep10/pep10.hpp"


namespace isa::pep10 {

// Convert an ELF binary into a working machine configuration
template<bool enable_history>
result<std::shared_ptr<isa::pep10::LocalMachine<enable_history>>> machine_from_elf(const ELFIO::elfio& image);

} // end namespace isa::pep10

#include "from_elf.tpp"
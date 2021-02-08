#pragma once

#include <list>
#include <string>

#include "masm/elf/section.hpp"
namespace masm {

enum class message_type
{
	kStatus,
	kWarning,
	kError,
};

struct message
{
	message_type type;
	std::string message;
};

class error_handler
{
	void register_error(void* section, uint32_t line, message message);
	std::list<std::tuple<uint32_t /*line*/, message> > errors_for_section(void* section);
	void clear_errors();
private:
	using message_t = std::tuple<void* /*section*/, uint32_t /*line*/, message /*message*/>
	std::list<message_t> messages;
};

} // End namespace masm
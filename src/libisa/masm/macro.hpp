#pragma once

#include <string>
#include <climits>

namespace masm {
/*
 * Macros originate from different places - user code, Qt resources, the operating system.
 * Macros may include other macros from the same level OR LOWER, but not higher.
 * This should help prevent undefined behavior in regards to user-built macros.
 *
 * User programs may include macros of any level, and the operating system may include
 * only core Macros
 */
enum class MacroType
{
    // These macros are shipped with the application and may never be changed at runtime.
    CoreMacro = 0,
    // System call macros are generated when the operating system is assembled.
    SystemMacro = 1,
    // User macros are found in a particular directory in the file system.
    UserMacro = 2,
	// Couldn't deduce macro type, which is the case when
	Unknown = 3
};

/*
 * A struct representing a macro entry.
 */
struct Macro
{
    MacroType type;
    std::string macro_name;
    uint8_t arg_count;
    std::string macro_text;
};
}; // End namespace masm
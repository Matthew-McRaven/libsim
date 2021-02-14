#pragma once

#include <string>

namespace asm::pep9
{
	class driver
	{
		void* assemble_user_program(std::string user_program, std::string operating_system, void* macro_registry);
		void* assembler_operating_system(std::string operating_system, void* macro_registry);
	private:
		void* sectionizer;
		void* preprocessor;
		void* assembler;
		void* intralinker;
		void* interlinker;
		void* annotater;
		void* sanitizer;
	}

} // End namespace asm::pep9
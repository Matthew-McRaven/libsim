#pragma once

#include "symbol/entry.hpp"

namespace masm::debug
{
	enum class mnemonic
	{
		kPushV, kPopV,
		kPushV, kPopF,
		kSwapE, kSwapB
	};
	enum class target
	{
		kHeap, kStack, kGlobals
	};
	struct trace_instruction
	{
		mnemonic mnemonic;
		target target;
		std::optional<void*> symbol;
	}

} // End namespace masm::debug
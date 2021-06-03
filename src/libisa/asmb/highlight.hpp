#pragma once

namespace asmb {
	enum class highlight_type
	{
		kInstructions,
		kMacro,
		kDotDirective,
		kSymbolDecl,
		kCharQuote,
		kStrQuote,
		kComment,
		kWarning,
		kError,
	};
}
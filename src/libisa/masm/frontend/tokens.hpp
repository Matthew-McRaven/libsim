#pragma once
namespace masm::frontend
{

enum class token_type
{
kError, kComma, kCharConstant, kComment, kDecConstant, kDotDirective,
kEmpty, kHexConstant, kIdentifier, kStrConstant, kSymbolDecl,
kMacroInvoke, kMacroArg
};

}
#include "parser.hpp"

#include <set>
#include <utility>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/algorithm/string.hpp>

#include "symbol/entry.hpp"
#include "masm/frontend/tokenizer.hpp"
#include "asmb/pep10/ir.hpp"
#include "isa/pep10/instruction.hpp"
#include "masm/ir/empty.hpp"
#include "masm/ir/directives.hpp"
#include "masm/ir/args.hpp"
//#include "masm/ir.macro.hpp"

bool asmb::pep10::requires_byte_operand(isa::pep10::instruction_mnemonic mn)
{
	return mn == isa::pep10::instruction_mnemonic::LDBA
		|| mn == isa::pep10::instruction_mnemonic::LDBX;
}
bool asmb::pep10::requires_addr_mode(isa::pep10::instruction_mnemonic)
{

}
std::optional<isa::pep10::addressing_mode> asmb::pep10::parse_addr_mode(std::string addr_mode)
{

}

bool asmb::pep10::allowed_addressing_mode(isa::pep10::instruction_mnemonic, isa::pep10::addressing_mode)
{

}
bool asmb::pep10::valid_symbol_name(const std::string& symbol)
{
	// TODO: Determine what makes a valid symbol;
	if(symbol.size() > 8) return false;
	return true;
}
auto asmb::pep10::parser::parse(
	std::shared_ptr<masm::project::project<uint16_t> >& project, 
	std::shared_ptr<masm::elf::code_section<uint16_t> >& section) -> bool
{
	using token_class_t = const std::set<masm::frontend::token_type>;
	static const token_class_t comment = {masm::frontend::token_type::kComment};
	static const token_class_t empty = {masm::frontend::token_type::kEmpty};
	static const token_class_t symbol = {masm::frontend::token_type::kSymbolDecl};
	static const token_class_t identifier = {masm::frontend::token_type::kIdentifier};
	static const token_class_t dot = {masm::frontend::token_type::kDotDirective};
	static const token_class_t macro = {masm::frontend::token_type::kMacroInvoke};

	bool success = true, had_dot_end = false;
	decltype(section->body_ir->ir_lines) ir_lines;
	for(auto [index, line] : section->body_token.value().tokens | boost::adaptors::indexed(0)) {
		auto start=line.begin(), last=line.end();
		bool local_success = true;
		std::string local_message = {};
		ir_pointer_t local_line = nullptr;
		std::shared_ptr<symbol::SymbolEntry<uint16_t>> local_symbol = nullptr;

		// Check if we comment or empty line. If so, we can skip a lot of processing.
		if(auto [match_comment, _1, text_comment] = masm::frontend::match(start, last, comment, true); match_comment) {
			auto [match_empty, _2, _3] = masm::frontend::match(start, last, empty, true); // Require that line end in empty token.
			// TODO: Process comment-only line.
			local_line = std::make_shared<masm::ir::comment_line<uint16_t>>();
			local_line->comment = text_comment;
			local_line->source_line = index;
			ir_lines.emplace_back(local_line);
			continue;
		}
		else if(auto [match_empty, _2, _3] = masm::frontend::match(start, last, empty, true); match_empty) {
			local_line = std::make_shared<masm::ir::blank_line<uint16_t>>();
			local_line->source_line = index;
			ir_lines.emplace_back(local_line);
			continue;
		}

		// Extract symbol declaration if present,
		if(auto [match_symbol, _1, text_symbol] = masm::frontend::match(start, last, symbol, true); match_symbol) {
			local_symbol = project->symbol_table->define(text_symbol);
		}

		// Begin parsing for mnemonics, dot commands, and macros.
		if(auto [match_ident, _1, text_identifier] = masm::frontend::match(start, last, identifier, true); match_ident) {
			// Check if identifier is a mnemonic. Otherwise, error.
			auto isa_def = isa::pep10::isa_definition::get_definition();
			auto it = std::find_if(isa_def.isa.cbegin(), isa_def.isa.cend(), 
				[ti=text_identifier](auto entry){
					return boost::iequals(isa::pep10::as_string(entry.first), ti);
				}
			);

			if(it == isa_def.isa.cend()) std::tie(local_success, local_message) = std::make_tuple(false, ";ERROR: Invalid instruction mnemonic.");
			else if(auto mnemonic = it->first; isa::pep10::is_opcode_unary(mnemonic)) {
				std::tie(local_success, local_message, local_line) = parse_unary(start, last, mnemonic);
			}
			else {
				std::tie(local_success, local_message, local_line) = parse_nonunary(start, last, 
					project->symbol_table, mnemonic);
			}

			success &= local_success;
			if(!local_success) {
				project->message_resolver->log_message(section, index, {masm::message_type::kError, local_message});
				continue;
			}
			else if(local_symbol) local_line->symbol_entry = local_symbol;
			
		}
		else if(auto [match_dot, _2, text_dot] = masm::frontend::match(start, last, dot, true); match_dot) {
			if(text_dot == "ADDRSS") std::tie(local_success, local_message, local_line) = parse_ADDRSS(start, last, project->symbol_table);
			else if(text_dot == "ASCII") std::tie(local_success, local_message, local_line) = parse_ASCII(start, last);
			else if(text_dot == "ALIGN") std::tie(local_success, local_message, local_line) = parse_ALIGN(start, last);
			else if(text_dot == "BLOCK") std::tie(local_success, local_message, local_line) = parse_BLOCK(start, last);
			else if(text_dot == "BURN") std::tie(local_success, local_message, local_line) = parse_BURN(start, last);
			else if(text_dot == "BYTE") std::tie(local_success, local_message, local_line) = parse_BYTE(start, last);
			else if(text_dot == "END") std::tie(local_success, local_message, local_line) = parse_END(start, last);
			else if(text_dot == "EQUATE") std::tie(local_success, local_message, local_line) = parse_EQUATE(start, last);
			else if(text_dot == "EXPORT") std::tie(local_success, local_message, local_line) = parse_EXPORT(start, last);
			else if(text_dot == "SYCALL") std::tie(local_success, local_message, local_line) = parse_SYCALL(start, last);
			else if(text_dot == "USYCALL") std::tie(local_success, local_message, local_line) = parse_USYCALL(start, last);
			else if(text_dot == "WORD") std::tie(local_success, local_message, local_line) = parse_WORD(start, last);
			else {
				std::tie(local_success, local_message) = std::make_tuple(false, ";ERROR: Invalid dot command.");
			}
			
			success &= local_success;
			if(!local_success) {
				project->message_resolver->log_message(section, index, {masm::message_type::kError, local_message});
				continue;
			}
			else if(local_symbol) local_line->symbol_entry = local_symbol;
			
			// Insert symbol declaration if present.
		}
		else if(auto [match_macro, _3, text_macro] = masm::frontend::match(start, last, macro, true); match_macro) {
			// Construct macro line.
			project->message_resolver->log_message(section, index, {masm::message_type::kError, ";ERROR: Macros currently unsupported."});
			success = false;
			continue;
			// Insert symbol declaration if present.
		}
		// We had an error, let's try and provide helpful messages for what went wrong.
		else {
			// Unexpected symbol declaration.
			if(local_symbol != nullptr) {
				project->message_resolver->log_message(section, index, {masm::message_type::kError, ";ERROR: Unexpected symbol declaration."});
				success = false;
			}
			// Unexpected end of line.
			else if(auto [match_empty, _1, _2 ] = masm::frontend::match(start, last, empty, true); match_empty) {
				project->message_resolver->log_message(section, index, {masm::message_type::kError, ";ERROR: Unexpected EOL."});
				success = false;
			}
			project->message_resolver->log_message(section, index, {masm::message_type::kError, ";ERROR: General parsing error."});
			success = false;
			continue;
		}
		
		if(auto [match_comment, _1, text_comment] = masm::frontend::match(start, last, comment, true); match_comment) {
			// Add comment to local line.
			local_line->comment = text_comment;
		}

		// ERROR! Line didn't end in a /n.
		if(auto [match_empty, _2, text_what] = masm::frontend::match(start, last, empty, true); !match_empty) {
			project->message_resolver->log_message(section, index, {masm::message_type::kError, fmt::format(";ERROR: Expected EOL, got {}", text_what)});
			success = false;
			continue;
		}

		local_line->source_line = index;
		ir_lines.emplace_back(local_line);
	}
	
	section->body_ir = masm::elf::code::ir<uint16_t>{};
	section->body_ir->ir_lines = ir_lines;
	return success;
}

std::tuple<bool, std::string, asmb::pep10::parser::arg_pointer_t > asmb::pep10::parser::parse_operand(
	masm::frontend::token_type token, std::string value, symbol_table_pointer_t symbol_table)
{
	char *end = nullptr;
	long long as_long = 0;
	switch(token)
	{
	case masm::frontend::token_type::kIdentifier:
		assert(symbol_table);
		if(!valid_symbol_name(value)) return {false, fmt::format(";ERROR: Invalid symbol \"{}\"", value) , nullptr};
		// TODO: Ban using mnemonics as symbols.
		else {
			auto symbol = symbol_table->reference(value);
			auto arg = std::make_shared<masm::ir::symbol_ref_argument<uint16_t>>(symbol);
			return {true, "", arg};
		}
	case masm::frontend::token_type::kCharConstant:
		// TODO: Range check values.
		if(masm::byte_string_length(value) <= 1) {
			return {true, "", std::make_shared<masm::ir::char_argument<uint16_t>>(value)};
		}
		else return {false, ";ERROR: Character literal may only be one byte." , nullptr};
	case masm::frontend::token_type::kStrConstant:
		// TODO: Range check values.
		if(masm::byte_string_length(value) <= 2) {
			return {true, "", std::make_shared<masm::ir::string_argument<uint16_t>>(value)};
		}
		else {
			return {true, "", std::make_shared<masm::ir::ascii_argument<uint16_t>>(value, 0xffff)};
		}
	case masm::frontend::token_type::kDecConstant:
		// TODO: Range check values.
		as_long = strtol(value.data(), &end, 10);
		if(end != &*value.end()) return {false, ";ERROR: Not a valid dec constant", nullptr};
		return {true, "",  std::make_shared<masm::ir::dec_argument<uint16_t>>(as_long)};
	case masm::frontend::token_type::kHexConstant:
		// TODO: Range check values.
		as_long = strtol(value.data(), &end, 16);
		if(end != &*value.end()) return {false, ";ERROR: Not a valid dec constant", nullptr};
		return {true, "",  std::make_shared<masm::ir::dec_argument<uint16_t>>(as_long)};
	default:
		return {false, {}, nullptr};
	}
}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_unary(
	token_iterator_t& start, const token_iterator_t& last,isa::pep10::instruction_mnemonic mn)
{
	auto ret_val = std::make_shared<asmb::pep10::unary_instruction>();
	ret_val->mnemonic = mn;
	return {true, {}, ret_val};
}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_nonunary(
	token_iterator_t& start, const token_iterator_t& last, symbol_table_pointer_t symbol_table,
	isa::pep10::instruction_mnemonic mn)
{
	using token_class_t = const std::set<masm::frontend::token_type>;
	static const token_class_t comma = {masm::frontend::token_type::kComma};
	static const token_class_t identifier = {masm::frontend::token_type::kIdentifier};
	static const token_class_t arg = {
		masm::frontend::token_type::kIdentifier, 
		masm::frontend::token_type::kHexConstant,
		masm::frontend::token_type::kDecConstant,
		masm::frontend::token_type::kStrConstant,
		masm::frontend::token_type::kCharConstant
	};

	auto ret_val = std::make_shared<asmb::pep10::nonunary_instruction>();
	ret_val->mnemonic = mn;
	if(auto [match_arg, token_arg, text_arg] = masm::frontend::match(start, last, arg, true); !match_arg) {
	}
	else if(auto [success_operand, err_operand, operand_value] = parse_operand(token_arg, text_arg, symbol_table); !success_operand){
		return {false, err_operand, nullptr};
	}
	else if(auto [match_comma, _1, _2] = masm::frontend::match(start, last, comma, true); requires_addr_mode(mn) && !match_comma) {
		return {false, ";ERROR: mnemonic requires addressing mode.", nullptr};
	}
	else if(auto [match_addr, _1, text_addr] = masm::frontend::match(start, last, identifier, true); !match_addr) {
		return {false, ";ERROR: Invalid addressing mode.", nullptr};
	}
	else if(auto addr = parse_addr_mode(text_addr); !addr) return {false, ";ERROR: Invalid addressing mode.", nullptr};
	else if(!allowed_addressing_mode(mn, addr.value())) {
		return {false, fmt::format(";ERROR: Illegal addressing mode \"{}\"", isa::pep10::as_string(addr.value())), nullptr};
	}
	// TODO: Explicitly enumerate allowed range in error.
	// The only time our operands must be 1 byte is if it is an immediate being loaded.
	else if(requires_byte_operand(mn) && addr == isa::pep10::addressing_mode::I && !operand_value->fits_in(1)) {
		return {false, ";ERROR: Operand must fit in 1 byte.", nullptr};
	}
	else if(!operand_value->fits_in(2)) return {false, ";ERROR: Operand must fit in 2 bytes.", nullptr};
	else {
		ret_val->argument = operand_value;
		ret_val->addressing_mode = addr.value();
	}
	return {true, {}, ret_val};
}

asmb::pep10::parser::ir_pointer_t asmb::pep10::parser::parse_macro_invocation(token_iterator_t& start, const token_iterator_t& last)
{
	using token_class_t = const std::set<masm::frontend::token_type>;

	return nullptr;
}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_ADDRSS(token_iterator_t& start, 
	const token_iterator_t& last, symbol_table_pointer_t symbol_table)
{
	using token_class_t = const std::set<masm::frontend::token_type>;
	static const token_class_t arg = {masm::frontend::token_type::kIdentifier};

	auto ret_val = std::make_shared<masm::ir::dot_address<uint16_t>>();
	if(auto [match_arg, token_arg, text_arg] = masm::frontend::match(start, last, arg, true); !match_arg) {
		return {false, ";ERROR: .ADDRSS requires a symbolic argument.", nullptr};
	}
	else if(auto [valid_operand, err_msg, argument] = parse_operand(token_arg, text_arg, symbol_table); !valid_operand) {
		return {false, err_msg, nullptr};
	}
	else {
		auto as_ref = std::dynamic_pointer_cast<masm::ir::symbol_ref_argument<uint16_t>>(argument);
		assert(as_ref);
		ret_val->argument = as_ref;
		return {true, "", ret_val};
	}
}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_ASCII(token_iterator_t& start, const token_iterator_t& last)
{
	using token_class_t = const std::set<masm::frontend::token_type>;
	static const token_class_t arg = {masm::frontend::token_type::kStrConstant};

	auto ret_val = std::make_shared<masm::ir::dot_ascii<uint16_t>>();
	if(auto [match_arg, _, text_arg] = masm::frontend::match(start, last, arg, true); !match_arg) {
		return {false, ";ERROR: .ASCII requires a string argument.", nullptr};
	}
	else {
		ret_val->argument = std::make_shared<masm::ir::ascii_argument<uint16_t>>(text_arg, 0xFFFF);
		return {true, "", ret_val};
	};
}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_ALIGN(token_iterator_t& start, const token_iterator_t& last)
{
	using token_class_t = const std::set<masm::frontend::token_type>;
	static const token_class_t arg = {
		masm::frontend::token_type::kDecConstant,
		masm::frontend::token_type::kHexConstant
	};

	auto ret_val = std::make_shared<masm::ir::dot_align<uint16_t>>();
	if(auto [match_arg, token_arg, text_arg] = masm::frontend::match(start, last, arg, true); !match_arg) {
		return {false, ";ERROR: .ALIGN requires a (hexa)decimal argument.", nullptr};
	}
	else if(auto [valid_operand, err_msg, argument] = parse_operand(token_arg, text_arg, nullptr); !valid_operand) {
		return {false, err_msg, nullptr};
	}
	else if(auto v=argument->value();v%2!=0 && v%4!=0 && v%8!=0) {
		return {false, ";ERROR: .ALIGN argument must be in {2,4,8}.", nullptr};
	}
	else {
		ret_val->argument = argument;
		return {true, "", ret_val};
	};
}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_BLOCK(token_iterator_t& start, const token_iterator_t& last)
{
	return {false, {}, nullptr};

}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_BURN(token_iterator_t& start, const token_iterator_t& last)
{
	return {false, {}, nullptr};
}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_BYTE(token_iterator_t& start, const token_iterator_t& last)
{
	return {false, {}, nullptr};
}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_END(token_iterator_t& start, const token_iterator_t& last)
{
	return {true, {}, std::make_shared<masm::ir::dot_end<uint16_t>>()};
}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_EQUATE(token_iterator_t& start, const token_iterator_t& last)
{
	return {false, {}, nullptr};

}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_EXPORT(token_iterator_t& start, const token_iterator_t& last)
{
	return {false, {}, nullptr};
}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_SYCALL(token_iterator_t& start, const token_iterator_t& last)
{
	return {false, {}, nullptr};
}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_USYCALL(token_iterator_t& start, const token_iterator_t& last)
{
	return {false, {}, nullptr};
}

std::tuple<bool, std::string, asmb::pep10::parser::ir_pointer_t> asmb::pep10::parser::parse_WORD(token_iterator_t& start, const token_iterator_t& last)
{
	using token_class_t = const std::set<masm::frontend::token_type>;
	static const token_class_t arg = {
		masm::frontend::token_type::kDecConstant,
		masm::frontend::token_type::kHexConstant,
		masm::frontend::token_type::kStrConstant,
		masm::frontend::token_type::kCharConstant
	};

	auto ret_val = std::make_shared<masm::ir::dot_word<uint16_t>>();
	if(auto [match_arg, token_arg, text_arg] = masm::frontend::match(start, last, arg, true); !match_arg) {
		return {false, ";ERROR: .WORD requires a literal argument.", nullptr};
	}
	else if(auto [valid_operand, err_msg, argument] = parse_operand(token_arg, text_arg, nullptr); !valid_operand) {
		return {false, err_msg, nullptr};
	}
	else if(!argument->fits_in(2)) {
		return {false, ";ERROR: Operand must be smaller than 2 bytes.", nullptr};
	}
	else {
		ret_val->argument = argument;
		return {true, "", ret_val};
	};
}
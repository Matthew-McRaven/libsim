#pragma once

#include "isa/pep10/instruction.hpp"
#include "masm/ir/base.hpp"
#include "masm/project/project.hpp"
#include "masm/project/section.hpp"
#include "masm/ir/args.hpp"

namespace asmb::pep10
{
	bool requires_byte_operand(isa::pep10::instruction_mnemonic);
	bool requires_addr_mode(isa::pep10::instruction_mnemonic);
	std::optional<isa::pep10::addressing_mode> parse_addr_mode(std::string addr_mode);
	bool allowed_addressing_mode(isa::pep10::instruction_mnemonic, isa::pep10::addressing_mode);
	bool valid_symbol_name(const std::string& symbol);

	class parser
	{
	public:
		using ir_pointer_t = std::shared_ptr<masm::ir::linear_line<uint16_t>>;
		using arg_pointer_t = std::shared_ptr<masm::ir::lir_argument<uint16_t>>;
		using symbol_table_pointer_t = std::shared_ptr<symbol::SymbolTable<uint16_t>>;
		virtual auto parse(
        	std::shared_ptr<masm::project::project<uint16_t> >& project, 
        	std::shared_ptr<masm::elf::code_section<uint16_t> >& section
    	) -> bool;
	private:
		std::tuple<bool, std::string, arg_pointer_t > parse_operand(masm::frontend::token_type token, 
			std::string value, symbol_table_pointer_t symbol_table);
		using token_iterator_t = masm::elf::code::token::line_t::iterator;
		std::tuple<bool, std::string, ir_pointer_t> parse_nonunary(token_iterator_t& start, 
			const token_iterator_t& last, symbol_table_pointer_t symbol_table, isa::pep10::instruction_mnemonic mn
		);
		std::tuple<bool, std::string, ir_pointer_t> parse_unary(token_iterator_t& start, 
			const token_iterator_t& last, isa::pep10::instruction_mnemonic mn
		);
		ir_pointer_t parse_macro_invocation(token_iterator_t& start, const token_iterator_t& last);

		std::tuple<bool, std::string, ir_pointer_t> parse_ASCII(token_iterator_t& start, const token_iterator_t& last);
		std::tuple<bool, std::string, ir_pointer_t> parse_ALIGN(token_iterator_t& start, const token_iterator_t& last);
		std::tuple<bool, std::string, ir_pointer_t> parse_BLOCK(token_iterator_t& start, const token_iterator_t& last);
		std::tuple<bool, std::string, ir_pointer_t> parse_BURN(token_iterator_t& start, const token_iterator_t& last);
		std::tuple<bool, std::string, ir_pointer_t> parse_BYTE(token_iterator_t& start, const token_iterator_t& last);
		std::tuple<bool, std::string, ir_pointer_t> parse_END(token_iterator_t& start, const token_iterator_t& last);
		std::tuple<bool, std::string, ir_pointer_t> parse_EQUATE(token_iterator_t& start, const token_iterator_t& last);
		std::tuple<bool, std::string, ir_pointer_t> parse_EXPORT(token_iterator_t& start, const token_iterator_t& last,
			symbol_table_pointer_t symbol_table);
		std::tuple<bool, std::string, ir_pointer_t> parse_SYCALL(token_iterator_t& start, const token_iterator_t& last);
		std::tuple<bool, std::string, ir_pointer_t> parse_USYCALL(token_iterator_t& start, const token_iterator_t& last);
		std::tuple<bool, std::string, ir_pointer_t> parse_WORD(token_iterator_t& start, const token_iterator_t& last,
			symbol_table_pointer_t symbol_table);
	};
} // End namespace asmb::pep10

#include "asmb/pep10/ir.hpp"

#include "symbol/entry.hpp"
#include "symbol/table.hpp"
/*
 * Unary Instruction
 */
asmb::pep10::unary_instruction::unary_instruction(): masm::ir::linear_line<uint16_t>()
{
    this->emits_object_code=true;
}
asmb::pep10::unary_instruction::unary_instruction(const asmb::pep10::unary_instruction& other):
    masm::ir::linear_line<uint16_t>(other), mnemonic(other.mnemonic)
{
    
}

asmb::pep10::unary_instruction &asmb::pep10::unary_instruction::operator=(
	asmb::pep10::unary_instruction other)
{
    swap(*this, other);
    return *this;
}


std::shared_ptr<masm::ir::linear_line<uint16_t>> asmb::pep10::unary_instruction::clone() const
{
    return std::make_shared<unary_instruction>(*this);
}

std::string asmb::pep10::unary_instruction::generate_listing_string() const
{
    // Potentially skip codegen
    std::string code_string = "";
    if(this->emits_object_code) {
        // TODO: Get correct mnemonic value.
		code_string = fmt::format("{:02X}", "");
    }

	return fmt::format("{:<6}{:>7}{}",
		fmt::format("0x{:04X}", this->base_address),
		code_string,
		generate_source_string()
	);
}

std::string asmb::pep10::unary_instruction::generate_source_string() const
{
    std::string symbol_string;
    if (this->symbol_entry != nullptr) {
        symbol_string = this->symbol_entry->name + ":";
    }
    // TODO: Get correct mnemonic string
    auto mnemonic_string = ".ADDRSS";
	std::string comment = this->comment.value_or("");
    return fmt::format("{:<9}{:<8}{:<12}{}",
		symbol_string,
		mnemonic_string,
		"",
		comment
	);

}

uint16_t asmb::pep10::unary_instruction::object_code_bytes() const
{
    return 1;
}

/*
 * Non-unary Instruction
 */
asmb::pep10::nonunary_instruction::nonunary_instruction(): masm::ir::linear_line<uint16_t>()
{
    this->emits_object_code=true;
}
asmb::pep10::nonunary_instruction::nonunary_instruction(const asmb::pep10::nonunary_instruction& other):
    masm::ir::linear_line<uint16_t>(other), mnemonic(other.mnemonic), addressing_mode(other.addressing_mode),
    argument(other.argument)
{
    
}

asmb::pep10::nonunary_instruction &asmb::pep10::nonunary_instruction::operator=(
	asmb::pep10::nonunary_instruction other)
{
    swap(*this, other);
    return *this;
}


std::shared_ptr<masm::ir::linear_line<uint16_t>> asmb::pep10::nonunary_instruction::clone() const
{
    return std::make_shared<nonunary_instruction>(*this);
}

std::string asmb::pep10::nonunary_instruction::generate_listing_string() const
{
    // Potentially skip codegen
    std::string code_string = "";
    if(this->emits_object_code) {
        // TODO: Get correct mnemonic value.
		code_string = fmt::format("{:02X}", "");
    }

	return fmt::format("{:<6}{:>7}{}",
		fmt::format("0x{:04X}", this->base_address),
		code_string,
		generate_source_string()
	);
}

std::string asmb::pep10::nonunary_instruction::generate_source_string() const
{
    std::string symbol_string;
    if (this->symbol_entry != nullptr) {
        symbol_string = this->symbol_entry->name + ":";
    }
    // TODO: Get correct mnemonic string
    // TODO: Get correct addressing mode string.
    auto mnemonic_string = ".ADDRSS";
	std::string comment = this->comment.value_or("");
    return fmt::format("{:<9}{:<8}{:<12}{}",
		symbol_string,
		mnemonic_string,
		"",
		comment
	);

}

uint16_t asmb::pep10::nonunary_instruction::object_code_bytes() const
{
    return 3;
}
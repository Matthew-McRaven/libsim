#include "directives.hpp"

#include <fmt/core.h>

#include "masm/conversion.hpp"
#include "symbol/table.hpp"

/*
 * .BLOCK
 */
template <typename address_size_t>
masm::ir::dot_address<address_size_t>::dot_address(const masm::ir::dot_address<address_size_t>& other)
{

}

template <typename address_size_t>
masm::ir::dot_address<address_size_t> &masm::ir::dot_address<address_size_t>::operator=(
	masm::ir::dot_address<address_size_t> other)
{
    swap(*this, other);
    return *this;
}


template <typename address_size_t>
std::shared_ptr<masm::ir::linear_line<address_size_t>> masm::ir::dot_address<address_size_t>::clone() const
{
    return std::make_shared<dot_address<address_size_t> >(*this);
}

template <typename address_size_t>
std::string masm::ir::dot_address<address_size_t>::generate_listing_string() const
{
    // Potentially skip codegen
    std::string code_string = "";
    if(this->emits_object_code) {
		code_string = fmt::format("{:04X}", this->argument->argument_value());
    }

	return fmt::format("{:<6}{:>7}{}",
		fmt::format("0x{:04X}", this->base_address),
		code_string,
		generate_source_string()
	);
}

template <typename address_size_t>
std::string masm::ir::dot_address<address_size_t>::generate_source_string() const
{
    std::string symbol_string;
    if (this->symbol_entry != nullptr) {
        symbol_string = this->symbol_entry->getName()+":";
    }
    auto dot_string = ".ADDRSS";
    auto operand_string = argument->argument_string();
	std::string comment = this->comment.value_or("");
    return fmt::format("{:<9}{:<8}{:<12}{}",
		symbol_string,
		dot_string,
		operand_string,
		comment
	);

}

template <typename address_size_t>
address_size_t masm::ir::dot_address<address_size_t>::object_code_bytes() const
{
    return 2;
}

template <typename address_size_t>
std::optional<std::shared_ptr<const symbol::SymbolEntry<address_size_t> > > masm::ir::dot_address<address_size_t>::symbolic_operand() const
{
    // The value of a .addrss instruction is always the value of another symbol.
    return argument.get()->symbol_value();
}

/*
 * .ALIGN
 */
template <typename address_size_t>
masm::ir::dot_align<address_size_t>::dot_align()
{
	this->emits_object_code = true;
}
template <typename address_size_t>
masm::ir::dot_align<address_size_t>::dot_align(const masm::ir::dot_align<address_size_t>& other)
{

}

template <typename address_size_t>
masm::ir::dot_align<address_size_t> &masm::ir::dot_align<address_size_t>::operator=(
	masm::ir::dot_align<address_size_t> other)
{
    swap(*this, other);
    return *this;
}


template <typename address_size_t>
std::shared_ptr<masm::ir::linear_line<address_size_t>> masm::ir::dot_align<address_size_t>::clone() const
{
    return std::make_shared<dot_align<address_size_t> >(*this);
}

template <typename address_size_t>
std::string masm::ir::dot_align<address_size_t>::generate_listing_string() const
{
    // Potentially skip codegen
    std::string code_string = "";
	auto bytes_remaining = this->num_bytes_generated();
	auto bytes_emitted = 0;

    while(this->emits_object_code && (bytes_remaining>0) && (bytes_emitted<3)) {		
		code_string.append("00");
		++bytes_emitted;
		--bytes_remaining;
    }

	auto temp = fmt::format("{:<6} {:<6}{}",
		fmt::format("0x{:04X}", this->base_address),
		code_string,
		generate_source_string()
	);

	while(bytes_remaining>0) {
		code_string = "";
		bytes_emitted = 0;
		while(this->emits_object_code && (bytes_remaining>0) && (bytes_emitted<3)) {		
			code_string.append("00");
			++bytes_emitted;
			--bytes_remaining;
		}
		
		temp.append(fmt::format("\n        {:<6}", code_string));
	}
	return temp;
}

template <typename address_size_t>
std::string masm::ir::dot_align<address_size_t>::generate_source_string() const
{
    std::string symbol_string;
    if (this->symbol_entry != nullptr) {
        symbol_string = this->symbol_entry->getName()+":";
    }
    auto dot_string = ".ALIGN";
    auto operand_string = argument->argument_string();
	std::string comment = this->comment.value_or("");
    return fmt::format("{:<9}{:<8}{:<12}{}",
		symbol_string,
		dot_string,
		operand_string,
		comment
	);

}

template <typename address_size_t>
address_size_t masm::ir::dot_align<address_size_t>::object_code_bytes() const
{
    return this->num_bytes_generated();
}

template <typename address_size_t>
address_size_t masm::ir::dot_align<address_size_t>::num_bytes_generated() const
{
	
	auto value = argument->argument_value();
    if(direction == AlignDirection::kTop) {
		return (value - (this->base_address % value)) % value;
	}
	else {
		throw std::logic_error("I don't know how to do this.");
	}
}

/*
 * .ASCII
 */
template <typename address_size_t>
masm::ir::dot_ascii<address_size_t>::dot_ascii()
{
	this->emits_object_code = true;
}
template <typename address_size_t>
masm::ir::dot_ascii<address_size_t>::dot_ascii(const masm::ir::dot_ascii<address_size_t>& other)
{

}

template <typename address_size_t>
masm::ir::dot_ascii<address_size_t> &masm::ir::dot_ascii<address_size_t>::operator=(
	masm::ir::dot_ascii<address_size_t> other)
{
    swap(*this, other);
    return *this;
}


template <typename address_size_t>
std::shared_ptr<masm::ir::linear_line<address_size_t>> masm::ir::dot_ascii<address_size_t>::clone() const
{
    return std::make_shared<dot_ascii<address_size_t> >(*this);
}

template <typename address_size_t>
std::string masm::ir::dot_ascii<address_size_t>::generate_listing_string() const
{
    // Potentially skip codegen
    std::string code_string = "";
	
	auto bytes_emitted = 0;

	decltype(std::string().substr({},{})) aliased = this->argument->argument_string();
	// Remove double quote character from either end of string.
	aliased = aliased.substr(1, aliased.length()-2);
	auto bytes = masm::byte_vector(aliased);
	auto bytes_head = bytes.begin();
    while(this->emits_object_code && (bytes_head!=bytes.end()) && (bytes_emitted<3)) {		
		code_string.append(fmt::format("{:02X}", *bytes_head++));
		++bytes_emitted;
    }

	auto temp = fmt::format("{:<6} {:<6}{}",
		fmt::format("0x{:04X}", this->base_address),
		code_string,
		generate_source_string()
	);
	
	while(bytes_head!=bytes.end()) {
		code_string = "";
		bytes_emitted = 0;
		while(this->emits_object_code && (bytes_head!=bytes.end()) && (bytes_emitted<3)) {		
			code_string.append(fmt::format("{:02X}", *bytes_head++));
			++bytes_emitted;
		}
		
		temp.append(fmt::format("\n        {:<6}", code_string));
	}
	return temp;
}

template <typename address_size_t>
std::string masm::ir::dot_ascii<address_size_t>::generate_source_string() const
{
    std::string symbol_string;
    if (this->symbol_entry != nullptr) {
        symbol_string = this->symbol_entry->getName()+":";
    }
    auto dot_string = ".ASCII";
    auto operand_string = argument->argument_string();
	std::string comment = this->comment.value_or("");
    return fmt::format("{:<9}{:<8}{:<12}{}",
		symbol_string,
		dot_string,
		operand_string,
		comment
	);

}

template <typename address_size_t>
address_size_t masm::ir::dot_ascii<address_size_t>::object_code_bytes() const
{
	decltype(std::string().substr({},{})) aliased = this->argument->argument_string();
	// Remove double quote character from either end of string.
	aliased = aliased.substr(1, aliased.length()-2);
    return masm::byte_string_length(aliased);
}

/*
 * .BLOCK
 */
template <typename address_size_t>
masm::ir::dot_block<address_size_t>::dot_block()
{
	this->emits_object_code = true;
}
template <typename address_size_t>
masm::ir::dot_block<address_size_t>::dot_block(const masm::ir::dot_block<address_size_t>& other)
{

}

template <typename address_size_t>
masm::ir::dot_block<address_size_t> &masm::ir::dot_block<address_size_t>::operator=(
	masm::ir::dot_block<address_size_t> other)
{
    swap(*this, other);
    return *this;
}


template <typename address_size_t>
std::shared_ptr<masm::ir::linear_line<address_size_t>> masm::ir::dot_block<address_size_t>::clone() const
{
    return std::make_shared<dot_block<address_size_t> >(*this);
}

template <typename address_size_t>
std::string masm::ir::dot_block<address_size_t>::generate_listing_string() const
{
    // Potentially skip codegen
    std::string code_string = "";
	
	auto bytes_emitted = 0;

	auto bytes = std::vector(this->argument->argument_value(), 0);
	auto bytes_head = bytes.begin();
    while(this->emits_object_code && (bytes_head!=bytes.end()) && (bytes_emitted<3)) {		
		code_string.append(fmt::format("{:02X}", *bytes_head++));
		++bytes_emitted;
    }

	auto temp = fmt::format("{:<6} {:<6}{}",
		fmt::format("0x{:04X}", this->base_address),
		code_string,
		generate_source_string()
	);
	
	while(bytes_head!=bytes.end()) {
		code_string = "";
		bytes_emitted = 0;
		while(this->emits_object_code && (bytes_head!=bytes.end()) && (bytes_emitted<3)) {		
			code_string.append(fmt::format("{:02X}", *bytes_head++));
			++bytes_emitted;
		}
		
		temp.append(fmt::format("\n        {:<6}", code_string));
	}
	return temp;
}

template <typename address_size_t>
std::string masm::ir::dot_block<address_size_t>::generate_source_string() const
{
    std::string symbol_string;
    if (this->symbol_entry != nullptr) {
        symbol_string = this->symbol_entry->getName()+":";
    }
    auto dot_string = ".BLOCK";
    auto operand_string = argument->argument_string();
	std::string comment = this->comment.value_or("");
    return fmt::format("{:<9}{:<8}{:<12}{}",
		symbol_string,
		dot_string,
		operand_string,
		comment
	);

}

template <typename address_size_t>
address_size_t masm::ir::dot_block<address_size_t>::object_code_bytes() const
{
	return this->argument->argument_value();
}

template <typename address_size_t>
bool masm::ir::dot_block<address_size_t>::tracks_trace_tags() const
{
	return true;
}

/*
 * .BURN
 */
template <typename address_size_t>
masm::ir::dot_burn<address_size_t>::dot_burn(const masm::ir::dot_burn<address_size_t>& other)
{

}

template <typename address_size_t>
masm::ir::dot_burn<address_size_t> &masm::ir::dot_burn<address_size_t>::operator=(
	masm::ir::dot_burn<address_size_t> other)
{
    swap(*this, other);
    return *this;
}


template <typename address_size_t>
std::shared_ptr<masm::ir::linear_line<address_size_t>> masm::ir::dot_burn<address_size_t>::clone() const
{
    return std::make_shared<dot_burn<address_size_t> >(*this);
}

template <typename address_size_t>
std::string masm::ir::dot_burn<address_size_t>::generate_listing_string() const
{
	auto temp = fmt::format("{:<6} {:<6}{}",
		fmt::format("0x{:04X}", this->base_address),
		"", // Doesn't generate any code!
		generate_source_string()
	);
	
	return temp;
}

template <typename address_size_t>
std::string masm::ir::dot_burn<address_size_t>::generate_source_string() const
{
    std::string symbol_string;
    if (this->symbol_entry != nullptr) {
        symbol_string = this->symbol_entry->getName()+":";
    }
    auto dot_string = ".BURN";
    auto operand_string = argument->argument_string();
	std::string comment = this->comment.value_or("");
    return fmt::format("{:<9}{:<8}{:<12}{}",
		symbol_string,
		dot_string,
		operand_string,
		comment
	);

}


/*
 * .BYTE
 */
template <typename address_size_t>
masm::ir::dot_byte<address_size_t>::dot_byte()
{
	this->emits_object_code = true;
}

template <typename address_size_t>
masm::ir::dot_byte<address_size_t>::dot_byte(const masm::ir::dot_byte<address_size_t>& other)
{

}

template <typename address_size_t>
masm::ir::dot_byte<address_size_t> &masm::ir::dot_byte<address_size_t>::operator=(
	masm::ir::dot_byte<address_size_t> other)
{
    swap(*this, other);
    return *this;
}


template <typename address_size_t>
std::shared_ptr<masm::ir::linear_line<address_size_t>> masm::ir::dot_byte<address_size_t>::clone() const
{
    return std::make_shared<dot_byte<address_size_t> >(*this);
}

template <typename address_size_t>
std::string masm::ir::dot_byte<address_size_t>::generate_listing_string() const
{
    // Potentially skip codegen
    std::string code_string = "";
	if(this->emits_object_code) {
		code_string = fmt::format("{:02X}", this->argument->argument_value() & 0xff);
	}


	auto temp = fmt::format("{:<6} {:<6}{}",
		fmt::format("0x{:04X}", this->base_address),
		code_string,
		generate_source_string()
	);
	
	return temp;
}

template <typename address_size_t>
std::string masm::ir::dot_byte<address_size_t>::generate_source_string() const
{
    std::string symbol_string;
    if (this->symbol_entry != nullptr) {
        symbol_string = this->symbol_entry->getName()+":";
    }
    auto dot_string = ".BYTE";
    auto operand_string = argument->argument_string();
	std::string comment = this->comment.value_or("");
    return fmt::format("{:<9}{:<8}{:<12}{}",
		symbol_string,
		dot_string,
		operand_string,
		comment
	);

}

template <typename address_size_t>
address_size_t masm::ir::dot_byte<address_size_t>::object_code_bytes() const
{
	return 1;
}

template <typename address_size_t>
bool masm::ir::dot_byte<address_size_t>::tracks_trace_tags() const
{
	return true;
}

/*
 * .BURN
 */
template <typename address_size_t>
masm::ir::dot_end<address_size_t>::dot_end(const masm::ir::dot_end<address_size_t>& other)
{

}

template <typename address_size_t>
masm::ir::dot_end<address_size_t> &masm::ir::dot_end<address_size_t>::operator=(
	masm::ir::dot_end<address_size_t> other)
{
    swap(*this, other);
    return *this;
}


template <typename address_size_t>
std::shared_ptr<masm::ir::linear_line<address_size_t>> masm::ir::dot_end<address_size_t>::clone() const
{
    return std::make_shared<dot_end<address_size_t> >(*this);
}

template <typename address_size_t>
std::string masm::ir::dot_end<address_size_t>::generate_listing_string() const
{
	auto temp = fmt::format("{:<6} {:<6}{}",
		"", // Doesn't have an address
		"", // Doesn't generate any code!
		generate_source_string()
	);
	
	return temp;
}

template <typename address_size_t>
std::string masm::ir::dot_end<address_size_t>::generate_source_string() const
{
    auto dot_string = ".END";
	std::string comment = this->comment.value_or("");
    return fmt::format("{:<9}{:<8}{:<12}{}",
		"",
		dot_string,
		"",
		comment
	);

}

/*
 * .EQUATE
 */

template <typename address_size_t>
masm::ir::dot_equate<address_size_t>::dot_equate(const masm::ir::dot_equate<address_size_t>& other)
{

}

template <typename address_size_t>
masm::ir::dot_equate<address_size_t> &masm::ir::dot_equate<address_size_t>::operator=(
	masm::ir::dot_equate<address_size_t> other)
{
    swap(*this, other);
    return *this;
}


template <typename address_size_t>
std::shared_ptr<masm::ir::linear_line<address_size_t>> masm::ir::dot_equate<address_size_t>::clone() const
{
    return std::make_shared<dot_equate<address_size_t> >(*this);
}

template <typename address_size_t>
std::string masm::ir::dot_equate<address_size_t>::generate_listing_string() const
{
	auto temp = fmt::format("{:<6} {:<6}{}",
		"",
		"",
		generate_source_string()
	);
	
	return temp;
}

template <typename address_size_t>
std::string masm::ir::dot_equate<address_size_t>::generate_source_string() const
{
    std::string symbol_string;
    if (this->symbol_entry != nullptr) {
        symbol_string = this->symbol_entry->getName()+":";
    }
    auto dot_string = ".EQUATE";
    auto operand_string = argument->argument_string();
	std::string comment = this->comment.value_or("");
    return fmt::format("{:<9}{:<8}{:<12}{}",
		symbol_string,
		dot_string,
		operand_string,
		comment
	);

}

template <typename address_size_t>
bool masm::ir::dot_equate<address_size_t>::tracks_trace_tags() const
{
	return true;
}

/*
 * .BYTE
 */
template <typename address_size_t>
masm::ir::dot_word<address_size_t>::dot_word()
{
	this->emits_object_code = true;
}

template <typename address_size_t>
masm::ir::dot_word<address_size_t>::dot_word(const masm::ir::dot_word<address_size_t>& other)
{

}

template <typename address_size_t>
masm::ir::dot_word<address_size_t> &masm::ir::dot_word<address_size_t>::operator=(
	masm::ir::dot_word<address_size_t> other)
{
    swap(*this, other);
    return *this;
}


template <typename address_size_t>
std::shared_ptr<masm::ir::linear_line<address_size_t>> masm::ir::dot_word<address_size_t>::clone() const
{
    return std::make_shared<dot_word<address_size_t> >(*this);
}

template <typename address_size_t>
std::string masm::ir::dot_word<address_size_t>::generate_listing_string() const
{
    // Potentially skip codegen
    std::string code_string = "";
	if(this->emits_object_code) {
		code_string = fmt::format("{:04X}", this->argument->argument_value());
	}


	auto temp = fmt::format("{:<6} {:<6}{}",
		fmt::format("0x{:04X}", this->base_address),
		code_string,
		generate_source_string()
	);
	
	return temp;
}

template <typename address_size_t>
std::string masm::ir::dot_word<address_size_t>::generate_source_string() const
{
    std::string symbol_string;
    if (this->symbol_entry != nullptr) {
        symbol_string = this->symbol_entry->getName()+":";
    }
    auto dot_string = ".BYTE";
    auto operand_string = argument->argument_string();
	std::string comment = this->comment.value_or("");
    return fmt::format("{:<9}{:<8}{:<12}{}",
		symbol_string,
		dot_string,
		operand_string,
		comment
	);

}

template <typename address_size_t>
address_size_t masm::ir::dot_word<address_size_t>::object_code_bytes() const
{
	return 2;
}

template <typename address_size_t>
bool masm::ir::dot_word<address_size_t>::tracks_trace_tags() const
{
	return true;
}
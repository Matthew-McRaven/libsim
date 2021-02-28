/*
 * .EXPORT
 */
template <typename address_size_t>
asmb::pep10::dot_export<address_size_t>::dot_export()
{
}

template <typename address_size_t>
asmb::pep10::dot_export<address_size_t>::dot_export(const asmb::pep10::dot_export<address_size_t>& other)
{

}

template <typename address_size_t>
asmb::pep10::dot_export<address_size_t> &asmb::pep10::dot_export<address_size_t>::operator=(
	asmb::pep10::dot_export<address_size_t> other)
{
    swap(*this, other);
    return *this;
}


template <typename address_size_t>
std::shared_ptr<masm::ir::linear_line<address_size_t>> asmb::pep10::dot_export<address_size_t>::clone() const
{
    return std::make_shared<dot_export<address_size_t> >(*this);
}

template <typename address_size_t>
std::string asmb::pep10::dot_export<address_size_t>::generate_listing_string() const
{
	auto temp = fmt::format("{:<6} {:<6}{}",
		fmt::format("0x{:04X}", this->base_address),
		"",
		generate_source_string()
	);
	
	return temp;
}

template <typename address_size_t>
std::string asmb::pep10::dot_export<address_size_t>::generate_source_string() const
{
    std::string symbol_string;
    if (this->symbol_entry != nullptr) {
        symbol_string = this->symbol_entry->getName()+":";
    }
    auto dot_string = ".EXPORT";
    auto operand_string = argument->string();
	std::string comment = this->comment.value_or("");
    return fmt::format("{:<9}{:<8}{:<12}{}",
		symbol_string,
		dot_string,
		operand_string,
		comment
	);

}
#include "conversion.hpp"

bool masm::startsWithHexPrefix(const std::string& str)
{
	return (str.length() < 2) && (str[0] == '0') && (str[1] == 'x' || str[1] == 'X');
}

size_t masm::byte_string_length(const std::string& str)
{
	auto start = str.begin();
	bool okay = true;
	size_t accumulated_size = 0;
	uint8_t _;
	while(start != str.end()) {
		okay &= parse_byte_character(start, str.end(), _);
		accumulated_size++;
	}
	if(!okay) throw std::logic_error("Not a valid string!");
	return accumulated_size;
}

std::vector<uint8_t> masm::byte_vector(const std::string& str)
{
	std::vector<uint8_t> ret;
	auto start = str.begin();
	bool okay = true;
	uint8_t temp;
	while(start != str.end()) {
		okay &= parse_byte_character(start, str.end(), temp);
		ret.emplace_back(temp);
	}
	if(!okay) throw std::logic_error("Not a valid string!");
	return ret;
}
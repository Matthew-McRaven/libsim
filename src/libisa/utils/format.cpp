
#include "format.hpp"

#include <fmt/core.h>

std::string utils::bytes_to_hex_string(const std::vector<uint8_t>& bytes, uint16_t bytes_per_line, bool include_zz)
{
	std::string hex_string = "";
    for (int i = 0; i < bytes.size(); i++) {
        hex_string +=(fmt::format("{:02x}", bytes[i]));
        hex_string += (((i % bytes_per_line) == bytes_per_line - 1) ? "\n" : " ");
    }
    if(include_zz) hex_string.append("zz");
	return hex_string;
}

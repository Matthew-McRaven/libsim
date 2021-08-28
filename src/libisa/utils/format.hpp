#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace utils
{
	std::string bytes_to_hex_string(const std::vector<uint8_t>& bytes, uint16_t bytes_per_line, bool include_zz = true);
}
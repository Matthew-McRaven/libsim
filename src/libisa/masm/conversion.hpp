#pragma once

#include <string>
#include <stdexcept>

namespace masm{

// Post: Returns true if str starts with the characters 0x or 0X. Otherwise returns false.
bool startsWithHexPrefix(const std::string& str)
{
	return (str.length() < 2) && (str[0] == '0') && (str[1] == 'x' || str[1] == 'X');
}


// Pre: str is a character or string stripped of its single or double quotes.
// Post: The sequence of characters representing the first possibly \ quoted character
// is stripped from the beginning of str.
// Post: value is the ASCII integer value of the first possibly \ quoted character.
template <typename value_size_t>
bool unqouted_string_to_integral(const std::string &str, int byte_length, value_size_t &value)
{
	// TODO: We need a real string library that handles UTF-8.

	static const auto max_chars= sizeof(value_size_t);
	auto current_char = 0;
	size_t index = 0;
	uint8_t temp = 0;

    if (startsWithHexPrefix(str)) {
		index+=2;
    }

	bool okay = true;

	while(okay) {
		if(index >= str.length()) break;
		else if (current_char >= max_chars || current_char>=byte_length) {
			okay = false;
			continue;
		}
		else if (str.substr(index, 1) == "\\") {
			if(str.length() < index+1) okay = false;
			else if (str.substr(index+1, 1) == "b") { // backspace
				temp = 8;
			}
			else if (str.substr(index+1, 1) == "f") { // form feed
				temp = 12;
			}
			else if (str.substr(index+1, 1) == "n") { // line feed (new line)
				temp = 10;
			}
			else if (str.substr(index+1, 1) == "r") { // carriage return
				temp = 13;
			}
			else if (str.substr(index+1, 1) == "t") { // horizontal tab
				temp = 9;
			}
			else if (str.substr(index+1, 1) == "v") { // vertical tab
				temp = 11;
			}
			// TODO: How do I handle hex string \x12?
			else {
				throw std::logic_error("I don't know where this was ever used!");
				temp = static_cast<uint8_t>('\\');
			}
		}
		else {
			temp = static_cast<uint8_t>(str[index++]);
    	}
		value = value << 8 | temp;
		++current_char;
    }
	return okay;
}


// Pre: str is enclosed in single (or double) quotes.
// Post: Returns the ASCII integer value of the character accounting for \ quoted characters.
template<typename value_size_t>
bool quoted_string_to_integral(const std::string& str,value_size_t& value )
{
	auto view = str.substr(1, str.size() - 2);
    return unqouted_string_to_integral(str, value);
}



// Pre: str is a double quoted string.
// Post: Returns the byte length of str accounting for possibly \ quoted characters.
size_t byte_string_length(std::string str);

}
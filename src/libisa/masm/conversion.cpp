#include "conversion.hpp"

bool masm::startsWithHexPrefix(const std::string& str)
{
	return (str.length() < 2) && (str[0] == '0') && (str[1] == 'x' || str[1] == 'X');
}

size_t masm::byte_string_length(const std::string& str)
{
	using substr_t = decltype(str.substr({},{}));
	substr_t aliased;
	if(str.substr(0, 1) == "\"" || str.substr(0, 1) == "'") {
		aliased = str.substr(1, str.length()-2);
	}
	else {
		aliased = str;
	}
    int length = 0;

    while (aliased.length() > 0) {
		if (aliased.find("\\x", 0) == 0 || aliased.find("\\X") == 0) {
			aliased = aliased.substr(4, aliased.length() - 4);
		}
        else if (aliased.find("\\") == 0) {
            aliased = aliased.substr(2, aliased.length() - 2);
        }
        else {
            aliased = aliased.substr(1, aliased.length() - 1);
        }
        length++;
    }
    return length;
;
}
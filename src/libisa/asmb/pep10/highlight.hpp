#pragma once

#include "asmb/highlight.hpp"
#include <string>
#include <map>

namespace asmb::pep10 {
	std::map<asmb::highlight_type, std::string> construct_rules();
}
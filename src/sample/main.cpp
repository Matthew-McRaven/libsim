#include <iostream>
#include<vector>
#include<iostream>
#include "version.hpp"

#include "masm/elf/serialize.hpp"
#include "asmb/pep10/highlight.hpp"
int main() {
	masm::elf::dump_dummy_file();
	masm::elf::dump_pep10_file();
	auto rules = asmb::pep10::construct_rules();
	for(auto it = rules.begin(); it != rules.end(); ++it) {
		std::cout << it->second;
	}
	std::cout << RTL_Model_VERSION_MAJOR << RTL_Model_VERSION_MINOR << std::endl;
}
#include <iostream>
#include<vector>
#include<iostream>
#include "version.hpp"

#include "masm/elf/serialize.hpp"

int main() {
	masm::elf::dump_dummy_file();
	masm::elf::dump_pep10_file();
	std::cout << RTL_Model_VERSION_MAJOR << RTL_Model_VERSION_MINOR << std::endl;
}
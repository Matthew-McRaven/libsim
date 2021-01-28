#include <iostream>
#include<vector>
#include<iostream>
#include "version.hpp"


#include <range/v3/all.hpp>
int main() {
	std::vector<int> ints = {0,1,2,3,4,5};
    auto even = [](int i) { return 0 == i % 2; };
    auto square = [](int i) { return i * i; };
	auto it = ints | ranges::views::filter(even) | ranges::views::transform(square);
	// "pipe" syntax of composing the views:
    for (int i : it) {
        std::cout << i << ' ';
    }

	std::cout << RTL_Model_VERSION_MAJOR << RTL_Model_VERSION_MINOR << std::endl;
}
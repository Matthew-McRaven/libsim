#include <array>
#include <iostream>

#include <catch.hpp>

#include "components/storage/block.hpp"
#include "components/storage/map.hpp"
#include "components/storage/range.hpp"

template<typename T> 
void helper(T& mem)
{
	// Make memory bigger & smaller to show it works.
	int sizes[3] = {8, 17, 5};
	auto size = 0;
	for (int it=0; it<2; it++) {
		size = sizes[it];
		// Check that we can increase, decrease size of memory.
		CHECK_NOTHROW(mem.resize(size));
		SECTION( "Check memory bounds" ) {
			CHECK(mem.max_offset() == size);
		}

		SECTION( "Check that we can read all in-bounds addresses" ) {
			for(auto it=0; it<size; it++) {
				CHECK_NOTHROW(mem.read_byte(it));
			}
		}

		// Can't catch exceptions in JS, so wrap it all.
		#ifndef EMSCRIPTEN
		SECTION( "Crash on OOB Read" ) {
			// We can read the last address.
			CHECK_NOTHROW(mem.read_byte(size));
			// But not one past that.
			CHECK_THROWS(mem.read_byte(size+1));
		}
		#endif

		SECTION( "Check that all bytes can be written" ) {
			for(auto it=0; it<size; it++) {
				CHECK_NOTHROW(mem.write_byte(it, it+8));
			}
			for(auto it=0; it<size; it++) {
				CHECK_NOTHROW(mem.read_byte(it) == it+8);
			}
		}

		SECTION( "Check that all bytes can be cleared." ) {
			mem.clear();
			for(auto it=0; it<size; it++) {
				CHECK_NOTHROW(mem.read_byte(it) == 0);
			}
		}
		
	}
}

TEST_CASE( "Validate block memory") {	
	auto mem = components::storage::storage_block<uint8_t>(0);
	helper(mem);
}

TEST_CASE( "Validate map memory") {	
	auto mem = components::storage::storage_map<uint8_t>(0, 0);
	helper(mem);
}

TEST_CASE( "Validate range memory") {	
	auto mem = components::storage::storage_range<uint8_t>(0, 0);
	helper(mem);
}
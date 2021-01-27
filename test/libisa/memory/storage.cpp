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
				CHECK_NOTHROW(mem.read(it));
			}
		}

		// Can't catch exceptions in JS, so wrap it all.
		#ifndef EMSCRIPTEN
		SECTION( "Crash on OOB Read" ) {
			// We can read the last address.
			CHECK_NOTHROW(mem.read(size));
			// But not one past that.
			CHECK_THROWS(mem.read(size+1));
		}
		SECTION( "Crash on OOB write" ) {
			// We can read the last address.
			CHECK_NOTHROW(mem.write(size, 10));
			// But not one past that.
			CHECK_THROWS(mem.write(size+1, 10));
		}
		#endif

		SECTION( "Check that all bytes can be written" ) {
			for(auto it=0; it<size; it++) {
				CHECK_NOTHROW(mem.write(it, it+8));
			}
			for(auto it=0; it<size; it++) {
				CHECK_NOTHROW(mem.read(it) == it+8);
			}
		}

		SECTION( "Check that all bytes can be cleared." ) {
			mem.clear();
			for(auto it=0; it<size; it++) {
				CHECK_NOTHROW(mem.read(it) == 0);
			}
		}
		
	}
}

TEST_CASE( "Validate storage systems") {
	using tp = components::storage::storage_base<uint8_t>;
	// Use shared points to automatically clean up our allocations.
	std::vector<std::shared_ptr<tp>> arr = {
		std::shared_ptr<tp>(new components::storage::storage_block<uint8_t>(0)),
		std::shared_ptr<tp>(new components::storage::storage_map<uint8_t>(0, 0)),
		std::shared_ptr<tp>(new components::storage::storage_range<uint8_t>(0, 0))
	}; 
	for(auto& store : arr) {
		helper(*store);
	}
}

#include <array>
#include <iostream>

#include <boost/range.hpp>
#include <catch.hpp>

#include "components/storage/block.hpp"
#include "components/storage/map.hpp"
#include "components/storage/range.hpp"

template<typename T> 
void helper(std::string type, T& mem)
{
	std::string suffix = fmt::format("[{}]", type);
	// Make memory bigger & smaller to show it works.
	std::array<int, 3> sizes= {8, 17, 5};
	for (auto size : sizes) {
		// Check that we can increase, decrease size of memory.
		CHECK_NOTHROW(mem.resize(size));
		DYNAMIC_SECTION( "Check memory bounds: " << suffix) {
			CHECK(mem.max_offset() == size);
		}

		DYNAMIC_SECTION( "Check that we can read all in-bounds addresses: " << suffix) {
			for(auto it=0; it<size; it++) {
				CHECK_NOTHROW(mem.read(it).value());
			}
		}

		// Can't catch exceptions in JS, so wrap it all.
		#ifndef EMSCRIPTEN
		DYNAMIC_SECTION( "Crash on OOB Read: " << suffix) {
			// We can read the last address.
			CHECK_NOTHROW(mem.read(size));
			// But not one past that.
			CHECK_THROWS(mem.read(size+1).value());
		}
		DYNAMIC_SECTION( "Crash on OOB write: " << suffix) {
			// We can read the last address.
			CHECK_NOTHROW(mem.write(size, 10));
			// But not one past that.
			CHECK_THROWS(mem.write(size+1, 10).value());
		}
		#endif

		DYNAMIC_SECTION( "Check that all bytes can be written: " << suffix) {
			for(auto it=0; it<size; it++) {
				CHECK_NOTHROW(mem.write(it, it+8));
			}
			for(auto it=0; it<size; it++) {
				auto i = mem.read(it);
				REQUIRE_FALSE(i.has_failure());
				CHECK_NOTHROW(i.value() == it+8);
			}
		}

		DYNAMIC_SECTION( "Check that all bytes can be cleared: " << suffix) {
			mem.clear();
			for(auto it=0; it<size; it++) {
				auto i = mem.read(it);
				REQUIRE_FALSE(i.has_failure());
				CHECK_NOTHROW(i.value() == 0);
			}
		}
		DYNAMIC_SECTION("Iterate over delta elements: " << suffix)
		{
			mem.clear();
			CHECK(mem.write(0, 0xfe).has_value());
			CHECK(mem.write(size-1, 0xed).has_value());
			auto delta = mem.take_delta().value();
			auto begin = delta->cbegin(), end = delta->cend();
			auto range = boost::iterator_range<decltype(begin)>(begin, end);
			for(auto&[storage, offset, value] : range){
				if(offset == 0) {CHECK(value == 0xfe);}
				else if (offset == size-1) {CHECK(value == 0xed);}
				else {REQUIRE(false);}
			}
		}
		DYNAMIC_SECTION("device_at: " << suffix)
		{
			CHECK(mem.device_at(0).value() == &mem);
			CHECK(mem.device_at(size-1).value() == &mem);
			CHECK(mem.device_at(size).value() == &mem);
			CHECK(mem.device_at(size+1).has_error());
		}
	}
}

TEST_CASE( "Validate storage systems") {
	using tp = components::storage::Base<uint8_t>;
	// Use shared points to automatically clean up our allocations.
	std::vector<std::tuple<std::string, std::shared_ptr<tp>>> arr = {
		{"Block", std::shared_ptr<tp>(new components::storage::Block<uint8_t>(0))},
		{"Map", std::shared_ptr<tp>(new components::storage::Map<uint8_t>(0, 0))},
		{"Range", std::shared_ptr<tp>(new components::storage::Range<uint8_t>(0, 0))}
	}; 
	for(auto& [type, store] : arr) {
		helper(type, *store);
	}
}


#include <array>
#include <iostream>

#include <boost/range.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <catch.hpp>

#include "components/storage/mmio.hpp"

TEST_CASE( "Validate memory-mapped output storage") {
	
	SECTION("Test configuration of Input")
	{
		// Create memory-mapped output storage.
		auto in = components::storage::Output<uint16_t, true>(0);
		// Require that memory-mapped output can only span one byte
		REQUIRE(in.max_offset() == 0);
		// And that it signals an out-of-bounds access when the offset 1+.
		REQUIRE(in.read(1).has_failure());
		REQUIRE(in.deltas_enabled());
	}

	SECTION("Allocate / write")
	{
		// Create memory-mapped input storage.
		auto in = components::storage::Output<uint16_t, true>(0);
		auto endpoint = in.endpoint();
		REQUIRE(in.write(0, 10).has_value());
		REQUIRE(in.write(0, 20).has_value());
		auto _1 = endpoint->next_value();
		REQUIRE(_1);
		CHECK(*_1 == 10);
		auto _2 = endpoint->next_value();
		REQUIRE(_2);
		CHECK(*_2 == 20);
	}

	SECTION("Allocate / write / unwrite")
	{
		// Create memory-mapped input storage.
		auto in = components::storage::Output<uint16_t, true>(0);
		auto endpoint = in.endpoint();
		REQUIRE(in.write(0, 10).has_value());
		REQUIRE(in.write(0, 20).has_value());
		auto _1 = endpoint->next_value();
		REQUIRE(_1);
		CHECK(*_1 == 10);
		auto _2 = endpoint->next_value();
		REQUIRE(_2);
		CHECK(*_2 == 20);
		// Undo any writes.
		in.take_delta().value()->apply_backward();
		auto _3 = endpoint->next_value();
		REQUIRE(_3);
		CHECK(*_3 == 0);
	}

	SECTION("Iterate over deltas.")
	{
		// Create memory-mapped input storage.
		auto in = components::storage::Output<uint16_t, true>(0);
		auto endpoint = in.endpoint();
		REQUIRE(in.write(0, 10).has_value());
		REQUIRE(in.write(0, 20).has_value());
		auto delta = in.take_delta().value();
		auto begin = delta->cbegin(), end = delta->cend();
		auto range = boost::iterator_range<decltype(begin)>(begin, end);
		int index = 0;
		for(auto &[storage, offset, value] : range){
			if(index == 0) {CHECK((int)value == 10);}
			else if(index == 1) {CHECK((int)value == 20);}
			else {REQUIRE(false);}
			index++;
		}
		REQUIRE(index == 2);
	}

}

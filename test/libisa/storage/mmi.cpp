#include <array>
#include <iostream>

#include <boost/range.hpp>
#include <catch.hpp>

#include "components/storage/mmio.hpp"

TEST_CASE( "Validate memory-mapped input storage") {

	SECTION("Test configuration of Input")
	{
		// Create memory-mapped input storage.
		auto in = components::storage::Input<uint16_t, true>(0);
		// Require that memory-mapped input can only span one byte
		REQUIRE(in.max_offset() == 0);
		// And that it signals an out-of-bounds access when the offset 1+.
		REQUIRE(in.read(1).has_failure());
		REQUIRE(in.deltas_enabled());
	}

	SECTION("Allocate / read")
	{
		// Create memory-mapped input storage.
		auto in = components::storage::Input<uint16_t, true>(0);
		auto endpoint = in.endpoint();
		endpoint->append_value(10);
		endpoint->append_value(20);
		// Check that a read->get yield the same value.
		// That is, get() doesn't advance state.
		// This is the correct behavior as per my discussion with Warford over discor on 20200718.
		auto _1 = in.read(0);
		REQUIRE_FALSE(_1.has_failure());
		CHECK(_1.value() == 10);
		auto _1_get = in.get(0);
		REQUIRE_FALSE(_1_get.has_failure());
		CHECK(_1_get.value() == 10);

		// Check that second read->get yield the same value.
		auto _2 = in.read(0);
		REQUIRE_FALSE(_2.has_failure());
		CHECK(_2.value() == 20);
		auto _2_get = in.get(0);
		REQUIRE_FALSE(_2_get.has_failure());
		CHECK(_2_get.value() == 20);
	}

	SECTION("Allocate / read / unread /read")
	{
		// Create memory-mapped input storage.
		auto in = components::storage::Input<uint16_t, true>(0);
		auto endpoint = in.endpoint();
		endpoint->append_value(10);
		endpoint->append_value(20);

		// Perform first read-check.
		auto _1 = in.read(0);
		REQUIRE_FALSE(_1.has_failure());
		CHECK(_1.value() == 10);

		// Perform second read-check.
		auto _2 = in.read(0);
		REQUIRE_FALSE(_2.has_failure());
		CHECK(_2.value() == 20);

		// Unread by backwarding delta, then read-check.
		auto delta = in.take_delta();
		REQUIRE(delta.has_value());
		delta.value()->apply_backward();
		auto _3 = in.read(0);
		REQUIRE_FALSE(_3.has_failure());
		CHECK(_3.value() == 10);
	}

	SECTION("Iterate over deltas.")
	{
		// Create memory-mapped input storage.
		auto in = components::storage::Input<uint16_t, true>(0);
		auto endpoint = in.endpoint();
		endpoint->append_value(10);
		endpoint->append_value(20);
		REQUIRE(in.read(0).has_value());
		REQUIRE(in.read(0).has_value());
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

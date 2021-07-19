#include <array>
#include <iostream>

#include <catch.hpp>

#include "components/storage/mmio.hpp"

TEST_CASE( "Validate memory-mapped input storage") {
	
	SECTION("Allocate / read")
	{
		// Create memory-mapped input storage.
		auto in = components::storage::Input<uint16_t, true>(0);
		auto endpoint = in.endpoint();
		endpoint->append_value(10);
		endpoint->append_value(20);
		auto _1 = in.read(0);
		REQUIRE_FALSE(_1.has_failure());
		CHECK(_1.value() == 10);
		auto _2 = in.read(0);
		REQUIRE_FALSE(_2.has_failure());
		CHECK(_2.value() == 20);
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

}

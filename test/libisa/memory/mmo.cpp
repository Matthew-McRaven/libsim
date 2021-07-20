#include <array>
#include <iostream>

#include <catch.hpp>

#include "components/storage/mmio.hpp"

TEST_CASE( "Validate memory-mapped output storage") {
	
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

}

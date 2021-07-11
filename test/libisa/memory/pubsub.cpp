#include <array>
#include <iostream>

#include <catch.hpp>

#include "components/storage/base.hpp"
#include "components/storage/pubsub.hpp"


TEST_CASE("Test pub-sub classes", "[components::storage]") {

	SECTION("Get an Endpoint, publish a value"){
		auto channel = std::make_shared<components::storage::Channel<uint8_t, uint8_t>>(0);
		auto endpoint = channel->new_endpoint();
		endpoint->append_value(0x25);
	}

	SECTION("Get an Endpoint, publish and read a  value"){
		auto channel = std::make_shared<components::storage::Channel<uint8_t, uint8_t>>(0);
		auto publish = channel->new_endpoint();
		auto subscribe = channel->new_endpoint();
		publish->append_value(0x25);
		auto value = subscribe->next_value();
		REQUIRE(value);
		CHECK(*value == 0x25);
	}

}

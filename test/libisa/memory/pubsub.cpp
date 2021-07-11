#include <array>
#include <iostream>

#include <catch.hpp>

#include "components/storage/base.hpp"
#include "components/storage/pubsub.hpp"


TEST_CASE("Test pub-sub classes", "[components::storage]") {

	SECTION("1prod/0consumer. Publish."){
		auto channel = std::make_shared<components::storage::Channel<uint8_t, uint8_t>>(0);
		auto endpoint = channel->new_endpoint();
		endpoint->append_value(0x25);
	}

	SECTION("1prod/1consumer. Publish, read."){
		auto channel = std::make_shared<components::storage::Channel<uint8_t, uint8_t>>(0);
		auto publish = channel->new_endpoint();
		auto subscribe = channel->new_endpoint();

		// Check that we can write/read a value.
		publish->append_value(0x25);
		auto value = subscribe->next_value();
		REQUIRE(value);
		CHECK(*value == 0x25);
	}

	SECTION("1prod/1consumer. Publish, revert, read."){
		auto channel = std::make_shared<components::storage::Channel<uint8_t, uint8_t>>(0);
		auto publish = channel->new_endpoint();
		auto subscribe = channel->new_endpoint();
		
		// Check that we can read a value from a singl producer and we can revert it.
		publish->append_value(0x25);
		auto value = subscribe->next_value();
		REQUIRE(value);
		CHECK(*value == 0x25);
		publish->unwrite();
		value = subscribe->next_value();
		REQUIRE(value);
		CHECK(*value == 0);
	}

	SECTION("2prod/1consumer. Publish, publish, revert, read."){
		auto channel = std::make_shared<components::storage::Channel<uint8_t, uint8_t>>(0);
		auto publish0 = channel->new_endpoint();
		auto publish1 = channel->new_endpoint();
		auto subscribe = channel->new_endpoint();
		publish0->append_value(0x25);
		publish1->append_value(0x10);

		// Read and check both values.
		auto value = subscribe->next_value();
		REQUIRE(value);
		CHECK(*value == 0x25);
		value = subscribe->next_value();
		REQUIRE(value);
		CHECK(*value == 0x10);

		// Check that we are reset to the root upon undoing publish0's write.
		publish0->unwrite();
		value = subscribe->next_value();
		REQUIRE(value);
		CHECK(*value == 0);
	}
	SECTION("2prod/1consumer. Publish, publish, unread, revert, read."){
		auto channel = std::make_shared<components::storage::Channel<uint8_t, uint8_t>>(0);
		auto publish0 = channel->new_endpoint();
		auto publish1 = channel->new_endpoint();
		auto subscribe0 = channel->new_endpoint();
		publish0->append_value(0x25);
		publish1->append_value(0x10);

		// Read and check both values.
		auto value = subscribe0->next_value();
		REQUIRE(value);
		CHECK(*value == 0x25);
		value = subscribe0->next_value();
		REQUIRE(value);
		CHECK(*value == 0x10);

		// Check that we can unread a value.
		subscribe0->unread();
		value = subscribe0->next_value();
		REQUIRE(value);
		CHECK(*value == 0x10);

		// Check that unwrite works after unread.
		publish0->unwrite();
		value = subscribe0->next_value();
		REQUIRE(value);
		CHECK(*value == 0);
	}

}

#include <array>
#include <iostream>

#include <catch.hpp>

#include "components/storage/layered.hpp"
#include "components/storage/mmio.hpp"
#include "components/storage/block.hpp"

TEST_CASE( "Validate layered storage") {
	
	SECTION("Allocate single storage. Read/Write Miss=Default/Ignore")
	{
		using storage_t = components::storage::Layered<uint16_t, true>;
		auto layered = storage_t(10, 0, storage_t::ReadMiss::kDefaultValue, storage_t::WriteMiss::kIgnore);
		auto backing_store = std::make_shared<components::storage::Block<uint16_t, true>>(8);
		layered.append_storage(1, backing_store);
		REQUIRE(layered.write(1, 15).has_value());
		// Check that both storage devices can access the written value.
		CHECK(backing_store->read(0).value() == 15);
		CHECK(layered.read(1).value() == 15);

		// Check that layered delta works for one storage device.
		layered.take_delta().value()->apply_backward();
		// Check that both storage devices can access the written value.
		CHECK(backing_store->read(0).value() == 0);
		CHECK(layered.read(1).value() == 0);

		// Check that locations without storage backing return a default value.
		CHECK(layered.read(0).value() == 0);
		CHECK(layered.read(10).value() == 0);

		// Check that a write to a backless area is ignored.
		REQUIRE(layered.write(0, 15).has_value());
		CHECK(layered.read(0).value() == 0);
	}

	SECTION("Allocate two storages. Test overlap. Read/Write Miss=Default/Ignore")
	{
		using storage_t = components::storage::Layered<uint16_t, true>;
		auto layered = storage_t(10, 0, storage_t::ReadMiss::kDefaultValue, storage_t::WriteMiss::kIgnore);
		auto backing_store0 = std::make_shared<components::storage::Block<uint16_t, true>>(2);
		auto backing_store1 = std::make_shared<components::storage::Block<uint16_t, true>>(8);
		layered.append_storage(0, backing_store0);
		layered.append_storage(1, backing_store1);
		REQUIRE(layered.write(1, 15).has_value());

		// Check that only the higher "priority" storage device is written.
		CHECK(backing_store0->read(1).value() == 15);
		CHECK(backing_store1->read(0).value() == 0);
		CHECK(layered.read(1).value() == 15);
	}

}

#include <array>
#include <iostream>

#include <boost/range.hpp>
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
		layered.append_storage(1, backing_store).value();
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
		layered.append_storage(0, backing_store0).value();
		layered.append_storage(1, backing_store1).value();
		REQUIRE(layered.write(1, 15).has_value());

		// Check that only the higher "priority" storage device is written.
		CHECK(backing_store0->read(1).value() == 15);
		CHECK(backing_store1->read(0).value() == 0);
		CHECK(layered.read(1).value() == 15);
	}

}

TEST_CASE( "Validate layered storage delta iterator") {
	
	SECTION("Allocate two storages, iterate over delta.")
	{
		using storage_t = components::storage::Layered<uint16_t, true>;
		auto layered = storage_t(10, 0, storage_t::ReadMiss::kDefaultValue, storage_t::WriteMiss::kIgnore);
		auto backing_store0 = std::make_shared<components::storage::Block<uint16_t, true>>(2);
		auto backing_store1 = std::make_shared<components::storage::Block<uint16_t, true>>(8);
		layered.append_storage(0, backing_store0).value();
		layered.append_storage(1, backing_store1).value();
		REQUIRE(layered.write(0, 0xde).has_value());
		REQUIRE(layered.write(1, 0xad).has_value());
		REQUIRE(layered.write(2, 0xbe).has_value());
		REQUIRE(layered.write(3, 0xef).has_value());
		auto delta = layered.take_delta().value();
		auto begin = delta->cbegin(), end = delta->cend();
		auto range = boost::iterator_range<decltype(begin)>(begin, end);
		for(auto&[storage, offset, value] : range){
			REQUIRE(storage == &layered);
			if(offset == 0) {CHECK(value == 0xde);}
			else if(offset == 1) {CHECK(value == 0xad);}
			else if(offset == 2) {CHECK(value == 0xbe);}
			else if(offset == 3) {CHECK(value == 0xef);}
		}
	}
}

TEST_CASE( "Validate layered storage device_at") {
	
	SECTION("Allocate two storages, check with device_at")
	{
		using storage_t = components::storage::Layered<uint16_t, true>;
		auto layered = storage_t(10, 0, storage_t::ReadMiss::kDefaultValue, storage_t::WriteMiss::kIgnore);
		auto backing_store0 = std::make_shared<components::storage::Block<uint16_t, true>>(2);
		auto backing_store1 = std::make_shared<components::storage::Block<uint16_t, true>>(8);
		layered.append_storage(0, backing_store0).value();
		layered.append_storage(1, backing_store1).value();
		CHECK(layered.device_at(0).value() == backing_store0.get());
		CHECK(layered.device_at(1).value() == backing_store0.get());
		CHECK(layered.device_at(2).value() == backing_store0.get());
		CHECK(layered.device_at(3).value() == backing_store1.get());
		CHECK(layered.device_at(8).value() == backing_store1.get());
		CHECK(layered.device_at(9).value() == backing_store1.get());
		CHECK(layered.device_at(10).has_error());
	}
}

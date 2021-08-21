#include <array>
#include <iostream>

#include <boost/range.hpp>
#include <catch.hpp>

#include "components/storage/block.hpp"
#include "components/storage/read_only.hpp"

TEST_CASE( "Validate ReadOnly storage adaptor.") {
	
	SECTION("Test configuration of ReadOnly")
	{
		// Create memory-mapped output storage.
		auto backing_storage = std::make_shared<components::storage::Block<uint16_t, true>>(10);
		auto storage = std::make_shared<components::storage::ReadOnly<uint16_t, true>>(backing_storage,
			components::storage::WriteAttemptPolicy::kIgnore);

		// Require that both storage devices believe they have the same number of bytes.
		REQUIRE(backing_storage->max_offset() == storage->max_offset());
		// And they agree in size after a resize.
		CHECK(storage->resize(20).has_value());
		REQUIRE(backing_storage->max_offset() == storage->max_offset());

		// Changes to backing_storage propogate to storage;
		CHECK(backing_storage->write(0x10, 0xDE).has_value());
		CHECK(backing_storage->read(0x10) == storage->read(0x10));
		CHECK(backing_storage->get(0x10) == storage->get(0x10));

		// Respects out of bounds reads.
		// We know from storage.cpp that OOB accesses in Block will raise an error.
		CHECK(backing_storage->read(0xFFFF) == storage->read(0xFFFF));

		// Set still works.
		CHECK(storage->set(0xA, 22).has_value());
		CHECK(backing_storage->read(0xA) == storage->read(0xA));
		CHECK(backing_storage->get(0xA) == storage->get(0xA));

		// Device at provides underlying storage.
		using storage_t = components::storage::ReadOnly<uint16_t, true, uint8_t>;
		CHECK(std::const_pointer_cast<const storage_t>(storage)->device_at(0).value() == backing_storage.get());
		CHECK(storage->device_at(0).value() == backing_storage.get());
	}

	SECTION("Write to ReadOnly, with policy kIgnore")
	{
		// Create memory-mapped output storage.
		auto backing_storage = std::make_shared<components::storage::Block<uint16_t, true>>(10);
		auto storage = std::make_shared<components::storage::ReadOnly<uint16_t, true>>(backing_storage,
			components::storage::WriteAttemptPolicy::kIgnore);

		// Write doesn't change value
		auto original_value = backing_storage->read(0xA);
		CHECK(storage->write(0xA, 22).has_value());
		CHECK(original_value == storage->read(0xA));
	}

	SECTION("Write to ReadOnly, with policy kFail")
	{
		// Create memory-mapped output storage.
		auto backing_storage = std::make_shared<components::storage::Block<uint16_t, true>>(10);
		auto storage = std::make_shared<components::storage::ReadOnly<uint16_t, true>>(backing_storage,
			components::storage::WriteAttemptPolicy::kFail);

		// Write doesn't change value
		auto value = storage->write(0xA, 22);
		REQUIRE(value.has_error());
		CHECK(value.error() == StorageErrc::IllegalWrite);
	}
}

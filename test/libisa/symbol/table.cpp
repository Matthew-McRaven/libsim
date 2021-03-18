#include "catch.hpp"

#include "symbol/value.hpp"
#include "symbol/table.hpp"
TEST_CASE( "Validate functionality of symbol table." ) {
	
	SECTION("Add symbol via reference(std::string).") {
		auto st = std::make_shared<symbol::table<uint16_t> >();
		auto x = st->reference("hello");
		// 2. 1 for local copy, 1 in map.
		CHECK(x.use_count() == 2);
	}

	SECTION("Find by name.") {
		auto st = std::make_shared<symbol::table<uint16_t> >();
		auto x = st->reference("hello");
		auto y = st->reference("hello");
		CHECK(x == y);
		CHECK(x->ID == y->ID);
	}

	SECTION("Require case matching.") {
		auto st = std::make_shared<symbol::table<uint16_t> >();
		auto x = st->reference("hello");
		auto y = st->reference("Hello");
		CHECK_FALSE(x == y);
		CHECK_FALSE(x->ID == y->ID);
	}

	SECTION("Singly referenced symbol marked for deletion.") {
		auto st = std::make_shared<symbol::table<uint16_t> >();
		st->reference("hello");
		CHECK(st->del("hello"));
	}

	SECTION("Multiply reference symbol marked for deletion.") {
		auto st = std::make_shared<symbol::table<uint16_t> >();
		auto x = st->reference("hello");
		CHECK_FALSE(st->del("hello"));
		CHECK(x.use_count() == 1);
	}

	SECTION("Symbol existence checks.") {
		auto st = std::make_shared<symbol::table<uint16_t> >();
		// Discard reference returned by st.
		st->reference("hello");
		CHECK_FALSE(st->exists("bye"));
		CHECK_FALSE(st->exists("Hello"));
		CHECK(st->exists("hello"));
	}

	SECTION("Undefined => defined => multiply defined" ) {
		auto st = std::make_shared<symbol::table<uint16_t> >();
		auto x = st->reference("hello");
		CHECK(x->state == symbol::definition_state::kUndefined);
		st->define(x->name);
		CHECK(x->state == symbol::definition_state::kSingle);
		st->define(x->name);
		CHECK(x->state == symbol::definition_state::kMultiple);
	}

	SECTION("Define before reference.") {
		auto st = std::make_shared<symbol::table<uint16_t> >();
		st->define("hello");
		auto x = st->reference("hello");
		CHECK(x->state == symbol::definition_state::kSingle);
	}

	for(int it=0; it<4; it++) {
		DYNAMIC_SECTION("Undefined count == " << it) {
			auto st = std::make_shared<symbol::table<uint16_t> >();
			for(int i=0;i<it;i++) st->reference("hello");
			CHECK(st->reference("hello")->state == symbol::definition_state::kUndefined);
		}
		if(it<2) continue;
		DYNAMIC_SECTION("Multiply defined count == " << it) {
			auto st = std::make_shared<symbol::table<uint16_t> >();
			for(int i=0; i<it;i++) st->define("hello");
			CHECK(st->reference("hello")->state == symbol::definition_state::kMultiple);
		}
	}

	SECTION("Offset modification, all.") {
		auto st = std::make_shared<symbol::table<uint16_t> >();
		auto x0 = st->define("h0");
		auto x1 = st->define("h1");
		auto x2 = st->define("h2");

		x0->value = std::make_shared<symbol::value_location<uint16_t>>(10, 0);
		x1->value = std::make_shared<symbol::value_location<uint16_t>>(20, 0);
		x2->value = std::make_shared<symbol::value_location<uint16_t>>(30, 0);
		symbol::set_offset<uint16_t>(st->entries(), 1, 10);

		CHECK(x0->value->value() == 11);
		CHECK(x1->value->value() == 21);
		CHECK(x2->value->value() == 31);
	}

	SECTION("Offset modification, threshold.") {
		auto st = std::make_shared<symbol::table<uint16_t> >();
		auto x0 = st->define("h0");
		auto x1 = st->define("h1");
		auto x2 = st->define("h2");

		x0->value = std::make_shared<symbol::value_location<uint16_t>>(10, 0);
		x1->value = std::make_shared<symbol::value_location<uint16_t>>(20, 0);
		x2->value = std::make_shared<symbol::value_location<uint16_t>>(30, 0);
		symbol::set_offset<uint16_t>(st->entries(), -1, 15);

		CHECK(x0->value->value() == 10);
		CHECK(x1->value->value() == 19);
		CHECK(x2->value->value() == 29);
	}



}
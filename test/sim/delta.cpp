#include <iostream>

#include "catch.hpp"
#include "magic_enum.hpp"

#include "isa/pep10/instruction.hpp"
#include "isa/pep10/block_computer.hpp"
#include "isa/step_delta.hpp"

using namespace isa::pep10;

TEST_CASE( "Test delta generation.", "[isa-sim]" ) {
	auto comp = isa::pep10::block_computer();

	// Init to clean slate.
	comp.clear_memory();
	comp.clear_regs();
	comp.clear_csrs();
	
    SECTION( "Test that registers generate deltas." ) {
		REQUIRE(comp.get_delta().delta_reg.size() == 0);
		auto const magic_num = 0xBEEF;
		for(auto enu : magic_enum::enum_values<isa::pep10::Registers>()) {
			if(enu == isa::pep10::Registers::MAX) break;
			comp.write_reg((int) enu, magic_num);
		}
		
		auto delta = comp.get_delta();
		// Check that each register has been changed.
		REQUIRE(delta.delta_reg.size() == 7);
		for(auto [name, vals]: delta.delta_reg) {
			auto [old_val, new_val] = vals;
			REQUIRE(old_val == 0 );
			REQUIRE(new_val == magic_num);
		}
	
		comp.clear_regs();
		// Reset didn't reach into existing delta and destroy it.
		REQUIRE(delta.delta_reg.size() == 7);
		delta = comp.get_delta();
		// Check that after being cleared, no registers have deltas.
		REQUIRE(delta.delta_reg.size() == 0);
		
    }

	SECTION( "Test that control status registers generate deltas." ) {
		REQUIRE(comp.get_delta().delta_csr.size() == 0);
		auto const magic_num = true;
		for(auto enu : magic_enum::enum_values<isa::pep10::CSR>()) {
			if(enu == isa::pep10::CSR::MAX) break;
			comp.write_csr((int) enu, magic_num);
		}
	
		auto delta = comp.get_delta();
		// Check that each register has been changed.
		REQUIRE(delta.delta_csr.size() == 4);
		for(auto [name, vals]: delta.delta_csr) {
			auto [old_val, new_val] = vals;
			REQUIRE(old_val == false );
			REQUIRE(new_val == magic_num);
		}
	
		comp.clear_csrs();
		// Reset didn't reach into existing delta and destroy it.
		REQUIRE(delta.delta_csr.size() == 4);
		delta = comp.get_delta();
		// Check that after being cleared, no registers have deltas.
		REQUIRE(delta.delta_csr.size() == 0);
		
    }

	SECTION( "Test memory delta, non-adjacent." ) {
		REQUIRE(comp.get_delta().memory_delta.size() == 0);
		auto const magic_num = 0xFE;
		std::map<uint16_t, uint8_t> pairs = {{0x01, 0xfe}, {0x03, 0xed}};
		for(auto [key, value] : pairs) {
			comp.write_mem_byte(key, value, false);
		}
	
		auto delta = comp.get_delta();

		// Check that each register has been changed.
		REQUIRE(delta.memory_delta.size() == pairs.size());
		for(auto [key, value] : pairs) {
			// Structured bindings can't be captured, must copy.
			auto x = key;
			using dm_t = isa::delta_memory<uint16_t>;
			auto idx = std::find_if(delta.memory_delta.begin(), delta.memory_delta.end(), 
				[x](dm_t v){
					return v.addr_span == std::tuple<uint16_t, uint16_t>{x,x};
					});
			REQUIRE(idx != delta.memory_delta.end());
			REQUIRE(idx->old_value[0] == 0);
			REQUIRE(idx->new_value[0] == value);
		}

	
		comp.clear_memory();
		// Reset didn't reach into existing delta and destroy it.
		REQUIRE(delta.memory_delta.size() == pairs.size());
		delta = comp.get_delta();
		// Check that after being cleared, no registers have deltas.
		REQUIRE(delta.memory_delta.size() == 0);
		
    }

	SECTION( "Test memory delta, monomerge." ) {
		REQUIRE(comp.get_delta().memory_delta.size() == 0);
		auto const magic_num = 0xFE;
		std::map<uint16_t, uint8_t> pairs = {{0x01, 0xfe}, {0x02, 0xe2}, {0x05, 0xda}, {0x04, 0xda},{0x03, 0xed}};
		for(auto [key, value] : pairs) {
			comp.write_mem_byte(key, value, false);
		}
	
		auto delta = comp.get_delta();

		// Check that each register has been changed.
		REQUIRE(delta.memory_delta.size() == 1);
		std::vector<uint8_t> results = {0xfe, 0xe2, 0xed, 0xda, 0xda};
		for(int it=0; it < 5; it++) {
			REQUIRE(delta.memory_delta[0].new_value[it] == results[it]);
			REQUIRE(delta.memory_delta[0].old_value[it] == 0);
		}

	
		comp.clear_memory();
		// Reset didn't reach into existing delta and destroy it.
		REQUIRE(delta.memory_delta.size() == 1);
		delta = comp.get_delta();
		// Check that after being cleared, no registers have deltas.
		REQUIRE(delta.memory_delta.size() == 0);
		
    }

}
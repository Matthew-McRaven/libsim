#include <iostream>

#include "catch.hpp"
#include "magic_enum.hpp"

#include "isa/pep9/proc.hpp"
#include "isa/pep9/instruction.hpp"
#include "isa/step_delta.hpp"

#include "components/reg/block.hpp"
#include "components/reg/historian.hpp"
#include "components/memory/block.hpp"
#include "components/memory/historian.hpp"
#include "components/computer/block_computer.hpp"

TEST_CASE( "pep/9 -- Test delta generation.", "[isa-sim]" ) {
	using namespace isa::pep9;
	auto regbank = components::reg::block_register<uint16_t, bool>(6, 4);
	auto hist_bank = components::reg::historian<decltype(regbank)>(regbank);
	auto memory = components::memory::block_memory<uint16_t>(0xffff);
	auto historian = components::memory::historian<decltype(memory), uint16_t>(memory);
	auto comp = components::computer::block_computer<isa::pep9::isa_processor, decltype(hist_bank), decltype(historian)>(regbank, memory);;

	// Init to clean slate.
	comp.clear_memory();
	comp.clear_regs();
	comp.clear_csrs();
	
    SECTION( "Test that registers generate deltas." ) {
		REQUIRE(comp.get_delta().delta_reg.size() == 0);
		auto const magic_num = 0xBEEF;
		for(auto enu : magic_enum::enum_values<isa::pep9::Registers>()) {
			comp.write_reg((int) enu, magic_num);
		}
		
		auto delta = comp.get_delta();
		// Check that each register has been changed.
		REQUIRE(delta.delta_reg.size() == 6);
		for(auto [name, vals]: delta.delta_reg) {
			auto [old_val, new_val] = vals;
			REQUIRE(old_val == 0 );
			REQUIRE(new_val == magic_num);
		}
	
		comp.clear_regs();
		// Reset didn't reach into existing delta and destroy it.
		REQUIRE(delta.delta_reg.size() == 6);
		delta = comp.get_delta();
		// Check that after being cleared, no registers have deltas.
		REQUIRE(delta.delta_reg.size() == 0);
		
    }

	SECTION( "Test that control status registers generate deltas." ) {
		REQUIRE(comp.get_delta().delta_csr.size() == 0);
		auto const magic_num = true;
		for(auto enu : magic_enum::enum_values<isa::pep9::CSR>()) {
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
		REQUIRE(comp.get_delta().delta_mem.size() == 0);
		auto const magic_num = 0xFE;
		std::map<uint16_t, uint8_t> pairs = {{0x01, 0xfe}, {0x03, 0xed}};
		for(auto [key, value] : pairs) {
			comp.write_mem_byte(key, value, false);
		}
	
		auto delta = comp.get_delta();

		// Check that each register has been changed.
		REQUIRE(delta.delta_mem.size() == pairs.size());
		for(auto [key, value] : pairs) {
			// Structured bindings can't be captured, must copy.
			auto x = key;
			using dm_t = components::memory::delta_memory<uint16_t>;
			auto idx = std::find_if(delta.delta_mem.begin(), delta.delta_mem.end(), 
				[x](dm_t v){
					return v.addr_span == std::tuple<uint16_t, uint16_t>{x,x};
					});
			REQUIRE(idx != delta.delta_mem.end());
			REQUIRE(idx->old_value[0] == 0);
			REQUIRE(idx->new_value[0] == value);
		}

	
		comp.clear_memory();
		// Reset didn't reach into existing delta and destroy it.
		REQUIRE(delta.delta_mem.size() == pairs.size());
		delta = comp.get_delta();
		// Check that after being cleared, no registers have deltas.
		REQUIRE(delta.delta_mem.size() == 0);
		
    }

	SECTION( "Test memory delta, monomerge." ) {
		REQUIRE(comp.get_delta().delta_mem.size() == 0);
		auto const magic_num = 0xFE;
		std::map<uint16_t, uint8_t> pairs = {{0x01, 0xfe}, {0x02, 0xe2}, {0x05, 0xda}, {0x04, 0xda},{0x03, 0xed}};
		for(auto [key, value] : pairs) {
			comp.write_mem_byte(key, value, false);
		}
	
		auto delta = comp.get_delta();

		// Check that each register has been changed.
		REQUIRE(delta.delta_mem.size() == 1);
		std::vector<uint8_t> results = {0xfe, 0xe2, 0xed, 0xda, 0xda};
		for(int it=0; it < 5; it++) {
			REQUIRE(delta.delta_mem[0].new_value[it] == results[it]);
			REQUIRE(delta.delta_mem[0].old_value[it] == 0);
		}

	
		comp.clear_memory();
		// Reset didn't reach into existing delta and destroy it.
		REQUIRE(delta.delta_mem.size() == 1);
		delta = comp.get_delta();
		// Check that after being cleared, no registers have deltas.
		REQUIRE(delta.delta_mem.size() == 0);
		
    }

}
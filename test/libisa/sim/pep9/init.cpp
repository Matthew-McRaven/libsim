#include <iostream>

#include "catch.hpp"
#include "magic_enum.hpp"

#include "isa/pep9/proc.hpp"
#include "isa/pep9/instruction.hpp"

#include "isa/step_delta.hpp"
#include "components/reg/block.hpp"
#include "components/memory/block.hpp"
#include "components/computer/block_computer.hpp"



TEST_CASE( "Test init of Pep/9 ISA.", "[isa-sim]" ) {
	using namespace isa::pep9;
	auto regbank = components::reg::block_register<uint16_t, bool>(7, 4);
	auto memory = components::memory::block_memory<uint16_t>(0xffff);
	auto comp = components::computer::block_computer<isa::pep9::isa_processor, decltype(regbank), decltype(memory)>(regbank, memory);

	// Init to clean slate.
	comp.clear_memory();
	comp.clear_regs();
	comp.clear_csrs();

    SECTION( "Set  & clear all registers." ) {
		// Test that registers can be set & read back.
		for(auto enu : magic_enum::enum_values<isa::pep9::Registers>()) {
			comp.write_reg((int) enu, (int) enu+1);
		}
		for(auto enu : magic_enum::enum_values<isa::pep9::Registers>()) {
			REQUIRE( comp.read_reg((int) enu) ==  (int) enu+1 );
		}

		// Test that clearing sets all to 0.
        comp.clear_regs();
		bool all_cleared = true;
		for(auto enu : magic_enum::enum_values<isa::pep9::Registers>()) {
			all_cleared &= comp.read_reg((int) enu) == 0;
		}
		REQUIRE( all_cleared );
    }

	SECTION( "Set  & clear all control status registers." ) {
		// Test that control/status registers can be set & read back.
		for(auto enu : magic_enum::enum_values<isa::pep9::CSR>()) {
			REQUIRE( comp.read_csr((int) enu) ==  false);
			comp.write_csr((int) enu, true);
		}
		for(auto enu : magic_enum::enum_values<isa::pep9::CSR>()) {
			REQUIRE( comp.read_csr((int) enu) ==  true);
		}

		// Test that clearing sets all to 0.
        comp.clear_csrs();
		bool all_cleared = true;
		for(auto enu : magic_enum::enum_values<isa::pep9::CSR>()) {
			all_cleared &= comp.read_csr((int) enu) == false;
		}
		REQUIRE( all_cleared );
    }

	SECTION( "Set  & clear all memory bytes." ) {
		// Test that all memory addresses can be written and read back.
		bool start_cleared = true;
		bool all_set = true;
		for(int it=0; it < 0x1'00'00; it++) {
			start_cleared &= comp.read_mem_byte(it, false) == 0;
			comp.write_mem_byte(it, it%256 ? it%256:1, false);
		}
		REQUIRE ( start_cleared );
		for(int it=0; it < 0x1'00'00; it++) {
			auto v = comp.read_mem_byte(it, false);
			all_set &= it%256 ? v == it%256 : v==1;
		}
		REQUIRE ( all_set );

		// Test that clearing sets all memory bytes to 0.
        comp.clear_memory();
		bool all_cleared = true;
		for(int it=0; it < 0x1'00'00; it++) {
			all_cleared &= comp.read_mem_byte(it, false) == 0;
		}
		REQUIRE( all_cleared );
    }
}
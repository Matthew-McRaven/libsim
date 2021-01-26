#include "catch.hpp"

#include "isa/pep9/instruction.hpp"

using namespace isa::pep9;

TEST_CASE( "pep/9 -- Sanity checks on trap instructions", "[isa-def-pep9]" ) {
    auto isa_def = isa::pep9::isa_definition::get_definition();
    
    SECTION( "NOP0" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::NOP0);
        REQUIRE( def->bit_pattern == 0b0010'0110 );
        REQUIRE( def->iformat == addressing_class::U_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::NOP0 );
        REQUIRE( as_string(def->mnemonic) == "NOP0" );
    }
	SECTION( "NOP1" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::NOP1);
        REQUIRE( def->bit_pattern == 0b0010'0111 );
        REQUIRE( def->iformat == addressing_class::U_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::NOP1 );
        REQUIRE( as_string(def->mnemonic) == "NOP1" );
    }
	SECTION( "NOP" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::NOP);
        REQUIRE( def->bit_pattern == 0b0010'1000 );
        REQUIRE( def->iformat == addressing_class::AAA_all );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::NOP );
        REQUIRE( as_string(def->mnemonic) == "NOP" );
    }
	SECTION( "DECI" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::DECI);
        REQUIRE( def->bit_pattern == 0b0011'0000 );
        REQUIRE( def->iformat == addressing_class::AAA_all );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = { true,  true,  true, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::DECI );
        REQUIRE( as_string(def->mnemonic) == "DECI" );
    }
	SECTION( "DECO" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::DECO);
        REQUIRE( def->bit_pattern == 0b0011'1000 );
        REQUIRE( def->iformat == addressing_class::AAA_all );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::DECO );
        REQUIRE( as_string(def->mnemonic) == "DECO" );
    }
	SECTION( "HEXO" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::HEXO);
        REQUIRE( def->bit_pattern == 0b0100'0000 );
        REQUIRE( def->iformat == addressing_class::AAA_all );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::HEXO );
        REQUIRE( as_string(def->mnemonic) == "HEXO" );
    }
	SECTION( "STRO" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::STRO);
        REQUIRE( def->bit_pattern == 0b01001'000 );
        REQUIRE( def->iformat == addressing_class::AAA_all );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::STRO );
        REQUIRE( as_string(def->mnemonic) == "STRO" );
    }
}
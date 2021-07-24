#include "catch.hpp"

#include "isa/pep9/pep9.hpp"

using namespace isa::pep9;

TEST_CASE( "pep/9 -- Sanity checks on Unary ISA instructions", "[isa-def-pep9]" ) {
    auto isa_def = isa::pep9::isa_definition::get_definition();
    
    SECTION( "STOP" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::STOP);
        REQUIRE( def->bit_pattern == 0b0000'0000 );
        REQUIRE( def->iformat == addressing_class::U_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::STOP );
        REQUIRE( as_string(def->mnemonic) == "STOP" );
    }

    SECTION( "RET" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::RET);
        REQUIRE( def->bit_pattern == 0b0000'0001 );
        REQUIRE( def->iformat == addressing_class::U_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::RET );
        REQUIRE( as_string(def->mnemonic) == "RET" );
    }

    SECTION( "RETTR" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::RETTR);
        REQUIRE( def->bit_pattern == 0b0000'0010);
        REQUIRE( def->iformat == addressing_class::U_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::RETTR );
        REQUIRE( as_string(def->mnemonic) == "RETTR" );
    }

    SECTION( "MOVSPA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::MOVSPA);
        REQUIRE( def->bit_pattern == 0b0000'0011 );
        REQUIRE( def->iformat == addressing_class::U_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::MOVSPA );
        REQUIRE( as_string(def->mnemonic) == "MOVSPA" );
    }

    SECTION( "MOVFLGA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::MOVFLGA);
        REQUIRE( def->bit_pattern == 0b0000'0100 );
        REQUIRE( def->iformat == addressing_class::U_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::MOVFLGA );
        REQUIRE( as_string(def->mnemonic) == "MOVFLGA" );
    }

    SECTION( "MOVAFLG" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::MOVAFLG);
        REQUIRE( def->bit_pattern == 0b0000'0101 );
        REQUIRE( def->iformat == addressing_class::U_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {true, true, true, true};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::MOVAFLG );
        REQUIRE( as_string(def->mnemonic) == "MOVAFLG" );
    }
}

TEST_CASE( "pep/9 -- Sanity checks on Unary `R`-type ISA instructions", "[isa-def-pep9]" ) {
    auto isa_def = isa::pep9::isa_definition::get_definition();

    SECTION( "NOTA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::NOTA);
        REQUIRE( def->bit_pattern == 0b0000'0110 );
        REQUIRE( def->iformat == addressing_class::R_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {true, true, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::NOTA );
        REQUIRE( as_string(def->mnemonic) == "NOTA" );
    }

    SECTION( "NOTX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::NOTX);
        REQUIRE( def->bit_pattern == 0b0000'0111);
        REQUIRE( def->iformat == addressing_class::R_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {true, true, false, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::NOTX );
        REQUIRE( as_string(def->mnemonic) == "NOTX" );
    }

    SECTION( "NEGA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::NEGA);
        REQUIRE( def->bit_pattern == 0b0000'1000 );
        REQUIRE( def->iformat == addressing_class::R_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {true, true, true, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::NEGA );
        REQUIRE( as_string(def->mnemonic) == "NEGA" );
    }

    SECTION( "NEGX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::NEGX);
        REQUIRE( def->bit_pattern == 0b0000'1001 );
        REQUIRE( def->iformat == addressing_class::R_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {true, true, true, false};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::NEGX );
        REQUIRE( as_string(def->mnemonic) == "NEGX" );
    }

    SECTION( "ASLA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ASLA);
        REQUIRE( def->bit_pattern == 0b0000'1010 );
        REQUIRE( def->iformat == addressing_class::R_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {true, true, true, true};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::ASLA );
        REQUIRE( as_string(def->mnemonic) == "ASLA" );
    }

    SECTION( "ASLX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ASLX);
        REQUIRE( def->bit_pattern == 0b0000'1011 );
        REQUIRE( def->iformat == addressing_class::R_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {true, true, true, true};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::ASLX );
        REQUIRE( as_string(def->mnemonic) == "ASLX" );
    }

    SECTION( "ASRA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ASRA);
        REQUIRE( def->bit_pattern == 0b0000'1100 );
        REQUIRE( def->iformat == addressing_class::R_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {true, true, false, true};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::ASRA );
        REQUIRE( as_string(def->mnemonic) == "ASRA" );
    }

    SECTION( "ASRX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ASRX);
        REQUIRE( def->bit_pattern == 0b0000'1101 );
        REQUIRE( def->iformat == addressing_class::R_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {true, true, false, true};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::ASRX );
        REQUIRE( as_string(def->mnemonic) == "ASRX" );
    }

    SECTION( "ROLA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ROLA);
        REQUIRE( def->bit_pattern == 0b0000'1110 );
        REQUIRE( def->iformat == addressing_class::R_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, true};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::ROLA );
        REQUIRE( as_string(def->mnemonic) == "ROLA" );
    }
    SECTION( "ROLX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ROLX);
        REQUIRE( def->bit_pattern == 0b0000'1111 );
        REQUIRE( def->iformat == addressing_class::R_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, true};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::ROLX );
        REQUIRE( as_string(def->mnemonic) == "ROLX" );
    }
    SECTION( "RORA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::RORA);
        REQUIRE( def->bit_pattern == 0b0001'0000 );
        REQUIRE( def->iformat == addressing_class::R_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, true};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::RORA );
        REQUIRE( as_string(def->mnemonic) == "RORA" );
    }
    SECTION( "RORX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::RORX);
        REQUIRE( def->bit_pattern == 0b0001'0001 );
        REQUIRE( def->iformat == addressing_class::R_none );
        bool CSR_modified[magic_enum::enum_count<isa::pep9::CSR>()] = {false, false, false, true};
        for(int it=0; it < magic_enum::enum_count<isa::pep9::CSR>(); it++) {
            REQUIRE( def->CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def->mnemonic == instruction_mnemonic::RORX );
        REQUIRE( as_string(def->mnemonic) == "RORX" );
    }
}
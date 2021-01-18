#include "catch.hpp"

#include "isa/pep9/defs.hpp"
#include "isa/pep9/instruction.hpp"

using namespace isa::pep9;

TEST_CASE( "pep/9 -- Sanity checks on Unary ISA instructions", "[isa-def-pep9]" ) {
    SECTION( "STOP" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::STOP];
        REQUIRE( def.bit_pattern == 0b0000'0000 );
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::STOP );
        REQUIRE( as_string(def.mnemonic) == "STOP" );
    }

    SECTION( "RET" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::RET];
        REQUIRE( def.bit_pattern == 0b0000'0001 );
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::RET );
        REQUIRE( as_string(def.mnemonic) == "RET" );
    }

    SECTION( "RETTR" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::RETTR];
        REQUIRE( def.bit_pattern == 0b0000'0010);
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::RETTR );
        REQUIRE( as_string(def.mnemonic) == "RETTR" );
    }

    SECTION( "MOVSPA" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::MOVSPA];
        REQUIRE( def.bit_pattern == 0b0000'0011 );
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::MOVSPA );
        REQUIRE( as_string(def.mnemonic) == "MOVSPA" );
    }

    SECTION( "MOVFLGA" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::MOVFLGA];
        REQUIRE( def.bit_pattern == 0b0000'0100 );
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::MOVFLGA );
        REQUIRE( as_string(def.mnemonic) == "MOVFLGA" );
    }

    SECTION( "MOVAFLG" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::MOVAFLG];
        REQUIRE( def.bit_pattern == 0b0000'0101 );
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::MOVAFLG );
        REQUIRE( as_string(def.mnemonic) == "MOVAFLG" );
    }
}

TEST_CASE( "pep/9 -- Sanity checks on Unary `R`-type ISA instructions", "[isa-def-pep9]" ) {
    SECTION( "NOTA" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::NOTA];
        REQUIRE( def.bit_pattern == 0b0000'0110 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::NOTA );
        REQUIRE( as_string(def.mnemonic) == "NOTA" );
    }

    SECTION( "NOTX" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::NOTX];
        REQUIRE( def.bit_pattern == 0b0000'0111);
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::NOTX );
        REQUIRE( as_string(def.mnemonic) == "NOTX" );
    }

    SECTION( "NEGA" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::NEGA];
        REQUIRE( def.bit_pattern == 0b0000'1000 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::NEGA );
        REQUIRE( as_string(def.mnemonic) == "NEGA" );
    }

    SECTION( "NEGX" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::NEGX];
        REQUIRE( def.bit_pattern == 0b0000'1001 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::NEGX );
        REQUIRE( as_string(def.mnemonic) == "NEGX" );
    }

    SECTION( "ASLA" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::ASLA];
        REQUIRE( def.bit_pattern == 0b0000'1010 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ASLA );
        REQUIRE( as_string(def.mnemonic) == "ASLA" );
    }

    SECTION( "ASLX" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::ASLX];
        REQUIRE( def.bit_pattern == 0b0000'1011 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ASLX );
        REQUIRE( as_string(def.mnemonic) == "ASLX" );
    }

    SECTION( "ASRA" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::ASRA];
        REQUIRE( def.bit_pattern == 0b0000'1100 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ASRA );
        REQUIRE( as_string(def.mnemonic) == "ASRA" );
    }

    SECTION( "ASRX" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::ASRX];
        REQUIRE( def.bit_pattern == 0b0000'1101 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ASRX );
        REQUIRE( as_string(def.mnemonic) == "ASRX" );
    }

    SECTION( "ROLA" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::ROLA];
        REQUIRE( def.bit_pattern == 0b0000'1110 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ROLA );
        REQUIRE( as_string(def.mnemonic) == "ROLA" );
    }
    SECTION( "ROLX" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::ROLX];
        REQUIRE( def.bit_pattern == 0b0000'1111 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ROLX );
        REQUIRE( as_string(def.mnemonic) == "ROLX" );
    }
    SECTION( "RORA" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::RORA];
        REQUIRE( def.bit_pattern == 0b0001'0000 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::RORA );
        REQUIRE( as_string(def.mnemonic) == "RORA" );
    }
    SECTION( "RORX" ) {
        auto& def = definition.isa[(int) instruction_mnemonic::RORX];
        REQUIRE( def.bit_pattern == 0b0001'0001 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::RORX );
        REQUIRE( as_string(def.mnemonic) == "RORX" );
    }
}
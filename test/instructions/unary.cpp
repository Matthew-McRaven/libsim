#include "catch.hpp"

#include "lib/instruction.hpp"

TEST_CASE( "Sanity checks on Unary ISA instructions", "[isa-def]" ) {
    SECTION( "RET" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::RET];
        REQUIRE( def.bit_pattern == 0b0000'0000 );
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::RET );
        REQUIRE( as_string(def.mnemonic) == "RET" );
    }

    SECTION( "SRET" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::SRET];
        REQUIRE( def.bit_pattern == 0b0000'0001);
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::SRET );
        REQUIRE( as_string(def.mnemonic) == "SRET" );
    }

    SECTION( "MOVSPA" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::MOVSPA];
        REQUIRE( def.bit_pattern == 0b0000'0010 );
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::MOVSPA );
        REQUIRE( as_string(def.mnemonic) == "MOVSPA" );
    }

    SECTION( "MOVASP" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::MOVASP];
        REQUIRE( def.bit_pattern == 0b0000'0011 );
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::MOVASP );
        REQUIRE( as_string(def.mnemonic) == "MOVASP" );
    }

    SECTION( "MOVFLGA" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::MOVFLGA];
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
        auto& def = isa.isa[(int) instruction_mnemonic::MOVAFLG];
        REQUIRE( def.bit_pattern == 0b0000'0101 );
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::MOVAFLG );
        REQUIRE( as_string(def.mnemonic) == "MOVAFLG" );
    }

    SECTION( "MOVTA" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::MOVTA];
        REQUIRE( def.bit_pattern == 0b0000'0110 );
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::MOVTA );
        REQUIRE( as_string(def.mnemonic) == "MOVTA" );
    }

    SECTION( "USCALL" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::USCALL];
        REQUIRE( def.bit_pattern == 0b0000'0111 );
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::USCALL );
        REQUIRE( as_string(def.mnemonic) == "USCALL" );
    }

    SECTION( "NOP" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::NOP];
        REQUIRE( def.bit_pattern == 0b0000'1000 );
        REQUIRE( def.iformat == addressing_class::U_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::NOP );
        REQUIRE( as_string(def.mnemonic) == "NOP" );
    }


}

TEST_CASE( "Sanity checks on Unary `R`-type ISA instructions", "[isa-def]" ) {
    SECTION( "NOTA" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::NOTA];
        REQUIRE( def.bit_pattern == 0b0001'0000 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::NOTA );
        REQUIRE( as_string(def.mnemonic) == "NOTA" );
    }

    SECTION( "NOTX" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::NOTX];
        REQUIRE( def.bit_pattern == 0b0001'0001);
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::NOTX );
        REQUIRE( as_string(def.mnemonic) == "NOTX" );
    }

    SECTION( "NEGA" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::NEGA];
        REQUIRE( def.bit_pattern == 0b0001'0010 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::NEGA );
        REQUIRE( as_string(def.mnemonic) == "NEGA" );
    }

    SECTION( "NEGX" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::NEGX];
        REQUIRE( def.bit_pattern == 0b0001'0011 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::NEGX );
        REQUIRE( as_string(def.mnemonic) == "NEGX" );
    }

    SECTION( "ASLA" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::ASLA];
        REQUIRE( def.bit_pattern == 0b0001'0100 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ASLA );
        REQUIRE( as_string(def.mnemonic) == "ASLA" );
    }

    SECTION( "ASLX" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::ASLX];
        REQUIRE( def.bit_pattern == 0b0001'0101 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ASLX );
        REQUIRE( as_string(def.mnemonic) == "ASLX" );
    }

    SECTION( "ASRA" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::ASRA];
        REQUIRE( def.bit_pattern == 0b0001'0110 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ASRA );
        REQUIRE( as_string(def.mnemonic) == "ASRA" );
    }

    SECTION( "ASRX" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::ASRX];
        REQUIRE( def.bit_pattern == 0b0001'0111 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ASRX );
        REQUIRE( as_string(def.mnemonic) == "ASRX" );
    }

    SECTION( "ROLA" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::ROLA];
        REQUIRE( def.bit_pattern == 0b0001'1000 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ROLA );
        REQUIRE( as_string(def.mnemonic) == "ROLA" );
    }
    SECTION( "ROLX" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::ROLX];
        REQUIRE( def.bit_pattern == 0b0001'1001 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ROLX );
        REQUIRE( as_string(def.mnemonic) == "ROLX" );
    }
    SECTION( "RORA" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::RORA];
        REQUIRE( def.bit_pattern == 0b0001'1010 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::RORA );
        REQUIRE( as_string(def.mnemonic) == "RORA" );
    }
    SECTION( "RORX" ) {
        auto& def = isa.isa[(int) instruction_mnemonic::RORX];
        REQUIRE( def.bit_pattern == 0b0001'1011 );
        REQUIRE( def.iformat == addressing_class::R_none );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::RORX );
        REQUIRE( as_string(def.mnemonic) == "RORX" );
    }
}
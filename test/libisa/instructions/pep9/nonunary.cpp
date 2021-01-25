#include "catch.hpp"

#include "isa/pep9/instruction.hpp"

using namespace isa::pep9;

TEST_CASE( "pep/9 -- Sanity checks on Non-Unary Branch ISA instructions", "[isa-def-pep9]" ) {
    auto isa_def = isa::pep9::isa_definition::get_definition();
    
    SECTION( "BR" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::BR);
        REQUIRE( def.bit_pattern == 0b0001'0010 );
        REQUIRE( def.iformat == addressing_class::A_ix );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::BR );
        REQUIRE( as_string(def.mnemonic) == "BR" );
    }

    SECTION( "BRLE" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::BRLE);
        REQUIRE( def.bit_pattern == 0b0001'0100);
        REQUIRE( def.iformat == addressing_class::A_ix );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::BRLE );
        REQUIRE( as_string(def.mnemonic) == "BRLE" );
    }

    SECTION( "BRLT" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::BRLT);
        REQUIRE( def.bit_pattern == 0b0001'0110 );
        REQUIRE( def.iformat == addressing_class::A_ix );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::BRLT );
        REQUIRE( as_string(def.mnemonic) == "BRLT" );
    }

    SECTION( "BREQ" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::BREQ);
        REQUIRE( def.bit_pattern == 0b0001'1000 );
        REQUIRE( def.iformat == addressing_class::A_ix );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::BREQ );
        REQUIRE( as_string(def.mnemonic) == "BREQ" );
    }

    SECTION( "BRNE" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::BRNE);
        REQUIRE( def.bit_pattern == 0b0001'1010 );
        REQUIRE( def.iformat == addressing_class::A_ix );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::BRNE );
        REQUIRE( as_string(def.mnemonic) == "BRNE" );
    }

    SECTION( "BRGE" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::BRGE);
        REQUIRE( def.bit_pattern == 0b0001'1100 );
        REQUIRE( def.iformat == addressing_class::A_ix );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::BRGE );
        REQUIRE( as_string(def.mnemonic) == "BRGE" );
    }

    SECTION( "BRGT" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::BRGT);
        REQUIRE( def.bit_pattern == 0b0001'1110 );
        REQUIRE( def.iformat == addressing_class::A_ix );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::BRGT );
        REQUIRE( as_string(def.mnemonic) == "BRGT" );
    }

    SECTION( "BRV" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::BRV);
        REQUIRE( def.bit_pattern == 0b0010'0000 );
        REQUIRE( def.iformat == addressing_class::A_ix );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::BRV );
        REQUIRE( as_string(def.mnemonic) == "BRV" );
    }

    SECTION( "BRC" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::BRC);
        REQUIRE( def.bit_pattern == 0b0010'0010 );
        REQUIRE( def.iformat == addressing_class::A_ix );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::BRC );
        REQUIRE( as_string(def.mnemonic) == "BRC" );
    }

    SECTION( "CALL" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::CALL);
        REQUIRE( def.bit_pattern == 0b0010'0100 );
        REQUIRE( def.iformat == addressing_class::A_ix );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::CALL );
        REQUIRE( as_string(def.mnemonic) == "CALL" );
    }
}


TEST_CASE( "pep/9 -- Sanity checks on Non-Unary ISA instructions", "[isa-def-pep9]" ) {
    auto isa_def = isa::pep9::isa_definition::get_definition();
    
    SECTION( "LDWA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::LDWA);
        REQUIRE( def.bit_pattern == 0b1100'0000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::LDWA );
        REQUIRE( as_string(def.mnemonic) == "LDWA" );
    }

    SECTION( "LDWX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::LDWX);
        REQUIRE( def.bit_pattern == 0b1100'1000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::LDWX );
        REQUIRE( as_string(def.mnemonic) == "LDWX" );
    }

    SECTION( "LDBA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::LDBA);
        REQUIRE( def.bit_pattern == 0b1101'0000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::LDBA );
        REQUIRE( as_string(def.mnemonic) == "LDBA" );
    }

    SECTION( "LDBX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::LDBX);
        REQUIRE( def.bit_pattern == 0b1101'1000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::LDBX );
        REQUIRE( as_string(def.mnemonic) == "LDBX" );
    }

    SECTION( "STWA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::STWA);
        REQUIRE( def.bit_pattern == 0b1110'0000 );
        REQUIRE( def.iformat == addressing_class::RAAA_noi );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::STWA );
        REQUIRE( as_string(def.mnemonic) == "STWA" );
    }

    SECTION( "STWX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::STWX);
        REQUIRE( def.bit_pattern == 0b1110'1000 );
        REQUIRE( def.iformat == addressing_class::RAAA_noi );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::STWX );
        REQUIRE( as_string(def.mnemonic) == "STWX" );
    }

    SECTION( "STBA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::STBA);
        REQUIRE( def.bit_pattern == 0b1111'0000 );
        REQUIRE( def.iformat == addressing_class::RAAA_noi );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::STBA );
        REQUIRE( as_string(def.mnemonic) == "STBA" );
    }

    SECTION( "STBX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::STBX);
        REQUIRE( def.bit_pattern == 0b1111'1000 );
        REQUIRE( def.iformat == addressing_class::RAAA_noi );
        bool CSR_modified[int(CSR::MAX)] = {false, false, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::STBX );
        REQUIRE( as_string(def.mnemonic) == "STBX" );
    }

	SECTION( "CPWA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::CPWA);
        REQUIRE( def.bit_pattern == 0b1010'0000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::CPWA );
        REQUIRE( as_string(def.mnemonic) == "CPWA" );
    }

	SECTION( "CPWX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::CPWX);
        REQUIRE( def.bit_pattern == 0b1010'1000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::CPWX );
        REQUIRE( as_string(def.mnemonic) == "CPWX" );
    }

	SECTION( "CPBA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::CPBA);
        REQUIRE( def.bit_pattern == 0b1011'0000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::CPBA );
        REQUIRE( as_string(def.mnemonic) == "CPBA" );
    }

	SECTION( "CPBX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::CPBX);
        REQUIRE( def.bit_pattern == 0b1011'1000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::CPBX );
        REQUIRE( as_string(def.mnemonic) == "CPBX" );
    }

	SECTION( "ADDA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ADDA);
        REQUIRE( def.bit_pattern == 0b0110'0000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ADDA );
        REQUIRE( as_string(def.mnemonic) == "ADDA" );
    }

	SECTION( "ADDX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ADDX);
        REQUIRE( def.bit_pattern == 0b0110'1000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ADDX );
        REQUIRE( as_string(def.mnemonic) == "ADDX" );
    }

	SECTION( "SUBA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::SUBA);
        REQUIRE( def.bit_pattern == 0b0111'0000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::SUBA );
        REQUIRE( as_string(def.mnemonic) == "SUBA" );
    }

	SECTION( "SUBX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::SUBX);
        REQUIRE( def.bit_pattern == 0b0111'1000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::SUBX );
        REQUIRE( as_string(def.mnemonic) == "SUBX" );
    }

	SECTION( "ANDA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ANDA);
        REQUIRE( def.bit_pattern == 0b1000'0000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ANDA );
        REQUIRE( as_string(def.mnemonic) == "ANDA" );
    }

	SECTION( "ANDX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ANDX);
        REQUIRE( def.bit_pattern == 0b1000'1000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ANDX );
        REQUIRE( as_string(def.mnemonic) == "ANDX" );
    }

	SECTION( "ORA" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ORA);
        REQUIRE( def.bit_pattern == 0b1001'0000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ORA );
        REQUIRE( as_string(def.mnemonic) == "ORA" );
    }

	SECTION( "ORX" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ORX);
        REQUIRE( def.bit_pattern == 0b1001'1000 );
        REQUIRE( def.iformat == addressing_class::RAAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, false, false};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ORX );
        REQUIRE( as_string(def.mnemonic) == "ORX" );
    }

	SECTION( "ADDSP" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::ADDSP);
        REQUIRE( def.bit_pattern == 0b0101'0000 );
        REQUIRE( def.iformat == addressing_class::AAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::ADDSP );
        REQUIRE( as_string(def.mnemonic) == "ADDSP" );
    }

	SECTION( "SUBSP" ) {
        auto& def = isa_def.isa.at(instruction_mnemonic::SUBSP);
        REQUIRE( def.bit_pattern == 0b0101'1000 );
        REQUIRE( def.iformat == addressing_class::AAA_all );
        bool CSR_modified[int(CSR::MAX)] = {true, true, true, true};
        for(int it=0; it < int(CSR::MAX); it++) {
            REQUIRE( def.CSR_modified[it] == CSR_modified[it]  );
        }
        REQUIRE( def.mnemonic == instruction_mnemonic::SUBSP );
        REQUIRE( as_string(def.mnemonic) == "SUBSP" );
    }
}
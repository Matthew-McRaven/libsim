#include "catch.hpp"

#include "masm/frontend/tokens.hpp"
#include "masm/frontend/tokenizer.hpp"
#include "asmb/pep10/tokenizer.hpp"
#include "masm/registry.hpp"
#include "masm/project/project.hpp"
#include "masm/project/init_project.hpp"
#include "masm/driver.hpp"

TEST_CASE( "Driver w/ tokenizer", "[asmb::pep10::tokenizer]"  ) {
    
	using tokenizer_t = asmb::pep10::tokenizer<masm::frontend::lexer_t>;
	using stage_t = masm::project::toolchain_stage;
	using driver_t = masm::driver<uint16_t, stage_t>;
	using project_t = driver_t::project_t;
	using section_t = driver_t::section_t;
	using transform_t = driver_t::transform_t;
	
	SECTION("Tokenize input using driver.", "[masm::driver]") {
		auto project = masm::project::init_project<uint16_t>();
		
		driver_t driver;

		auto file = std::make_shared<masm::project::source_file>();
		file->name = "main";
		file->body = "LWDA 20,d\n.END\n";

		masm::frontend::tokenizer<uint16_t, tokenizer_t> tokenizer{};
		transform_t tx_tokenizer = [&tokenizer](project_t& proj, std::list<driver_t::work_t>& work) {
			bool success = true;
			driver_t::work_iterable_t result_work;
			std::transform(work.begin(), work.end(), std::back_inserter(result_work),[&](auto& value){
				success &= tokenizer.tokenize(proj, std::get<driver_t::section_t>(value));
				return driver_t::work_iterable_t::value_type{stage_t::TOKEN, value};
			});
			return driver_t::result_t{success, result_work};
		};
		driver.register_transform(tx_tokenizer, stage_t::RAW);

		auto res = driver.assemble_os(project, file, masm::project::toolchain_stage::TOKEN);
		CHECK(res.first);
		auto tokens = project->images[0]->os->body_token;
		CHECK(tokens);
		CHECK(tokens.value().tokens[0].size() == 5);
		CHECK(tokens.value().tokens[0][0].first == masm::frontend::token_type::kIdentifier);
		CHECK(tokens.value().tokens[0][1].first == masm::frontend::token_type::kDecConstant);
		CHECK(tokens.value().tokens[0][2].first == masm::frontend::token_type::kComma);
		CHECK(tokens.value().tokens[0][3].first == masm::frontend::token_type::kIdentifier);
		CHECK(tokens.value().tokens[0][4].first == masm::frontend::token_type::kEmpty);

		CHECK(tokens.value().tokens[1].size() == 2);
		CHECK(tokens.value().tokens[1][0].first == masm::frontend::token_type::kDotDirective);
		CHECK(tokens.value().tokens[1][1].first == masm::frontend::token_type::kEmpty);
	}
}
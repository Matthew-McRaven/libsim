#pragma once

#include "masm/project/section.hpp"
#include "masm/project/project.hpp"
#include "masm/frontend/sectionize.hpp"
#include "masm/frontend/tokenizer.hpp"
#include "masm/frontend/preprocesser.hpp"

namespace masm 
{
	/**
	 * A driver is responsible for taking multiple different pieces of the assembler and "chaining" them together.
	 * It is a pipeline, where transformations move a piece of data (part of a program) from one state to another.
	 * These transformations should be steps in the assembler, like the preprocessor, parser, or linker.
	 */
	template <typename address_size_t, typename stage_t>
	class driver
	{
	public:
		using result_t = std::pair<stage_t, bool>;
		using project_t = std::shared_ptr<masm::project::project<address_size_t> >;
		using image_t = std::shared_ptr<masm::elf::image<address_size_t> >;
		using section_t = std::shared_ptr<masm::elf::code_section<address_size_t> >;
		using transform_t = std::function<result_t(project_t&, section_t&)>;

		/**
		 * Upon assembling a project, all sections are collated into the project's work queue.
		 * Sections at the lowest stage are pulled from the queue, and the driver searches for a transformation matching that stage.
		 * If a transformation, the section will be run through the transformer and re-queued on the work queue with the section's new stage.
		 * Execution continues until all sections in the project reach the `target_stage.` 
		 */
		std::pair<bool, std::string> assemble_project(std::shared_ptr<masm::project::project<address_size_t> >& project, 
			std::vector<masm::project::source_file> sources, stage_t target_stage);
		bool register_transform(transform_t, stage_t);
	private:
		std::map<stage_t, transform_t> transforms_;
	};
} // End namespace masm
#include "driver.tpp"
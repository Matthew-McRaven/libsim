#include "pack.hpp"
#include <iostream>
#include <sstream>
#include <elfio/elfio.hpp>
#include "masm/project/section.hpp"
#include "symbol/types.hpp"
template <typename addr_size_t>
auto masm::elf::pack_image(std::shared_ptr<masm::project::project<addr_size_t> >& project, 
	std::shared_ptr<masm::elf::image<addr_size_t> >& image
) -> std::pair<bool, std::istream&&>
{
	std::stringstream stream;
	using namespace ELFIO;
	elfio writer;
	
	writer.create( ELFCLASS32, ELFDATA2LSB );
	writer.set_os_abi( ELFOSABI_LINUX );
	writer.set_type( ET_REL );
	writer.set_machine( 0x8088 );
	// Write code sections.
	for(auto section : image->sections) {
		auto elf_section = writer.sections.add( section->header.name );
		elf_section->set_type( SHT_PROGBITS );
		elf_section->set_flags( SHF_ALLOC | SHF_EXECINSTR | SHF_WRITE);
		elf_section->set_addr_align( 0x1 );
		auto as_code_section = std::static_pointer_cast<const masm::elf::code_section<addr_size_t>>(section);
		auto as_byte_vec = masm::elf::to_bytes(as_code_section);
		elf_section->set_data((const char*)as_byte_vec.data(), as_byte_vec.size());
	}

	// Write symbol table.
	auto symbols = image->symbol_table->entries();
    auto str_tab = writer.sections.add( ".strtab" );
    str_tab->set_type( SHT_STRTAB );
	// Create string table writer
    string_section_accessor str_ac( str_tab );

	// Create symbol table section
    auto sym_tab = writer.sections.add( ".symtab" );
    sym_tab->set_type( SHT_SYMTAB );
    sym_tab->set_info(1+std::distance(cbegin(symbols), cend(symbols)));
    sym_tab->set_addr_align( 0x2 );
    sym_tab->set_entry_size( writer.get_default_entry_size( SHT_SYMTAB ) );
    sym_tab->set_link( str_tab->get_index() );
    // Create symbol table writer
    symbol_section_accessor sym_ac( writer, sym_tab );
	for(auto symbol : symbols) {
		auto binding = STB_LOCAL;
		if(symbol->binding == symbol::binding_t::kGlobal) binding = STB_GLOBAL;
		else if(symbol->binding == symbol::binding_t::kWeak) binding = STB_WEAK;
		// TODO: Handle types, section pointer correctly.
		sym_ac.add_symbol( str_ac, symbol->name.data(), symbol->value->value(), 0, binding, STT_NOTYPE, 0, SHN_ABS);
	}
	// Do not generate relocation entries, since Pep/9 and 10 programs are not relocatable.
	if(false) writer.set_entry( 0xfefe );
	writer.save(stream);

	return {true, std::move(stream)};
}
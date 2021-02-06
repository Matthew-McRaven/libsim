#include "serialize.hpp"	

#include <elfio/elfio.hpp>

void masm::elf::dump_dummy_file()
{
	using namespace ELFIO;
	elfio writer;
	
	writer.create( ELFCLASS32, ELFDATA2LSB );
	writer.set_os_abi( ELFOSABI_LINUX );
	writer.set_type( ET_EXEC );
	writer.set_machine( EM_X86_64 );
	section* text_sec = writer.sections.add( ".text" );
	text_sec->set_type( SHT_PROGBITS );
	text_sec->set_flags( SHF_ALLOC | SHF_EXECINSTR );
	text_sec->set_addr_align( 0x10 );
	char text[] = { '\xB8', '\x04', '\x00', '\x00', '\x00',   // mov eax, 4
	'\xBB', '\x01', '\x00', '\x00', '\x00',   // mov ebx, 1
	'\xB9', '\x20', '\x80', '\x04', '\x08',   // mov ecx, msg 
	'\xBA', '\x0E', '\x00', '\x00', '\x00',   // mov edx, 14 
	'\xCD', '\x80',                           // int 0x80'\xB8', 
	'\x01', '\x00', '\x00', '\x00',   // mov eax, 1
	'\xCD', '\x80' };// int 0x80
	text_sec->set_data( text, sizeof( text ) );
	segment* text_seg = writer.segments.add();
	text_seg->set_type( PT_LOAD );
	text_seg->set_virtual_address( 0x08048000 );
	text_seg->set_physical_address( 0x08048000 );
	text_seg->set_flags( PF_X| PF_R );
	text_seg->set_align( 0x1000 );
	text_seg->add_section_index( text_sec->get_index(),text_sec->get_addr_align() );

	section* data_sec = writer.sections.add( ".data" );
	data_sec->set_type( SHT_PROGBITS );
	data_sec->set_flags( SHF_ALLOC | SHF_WRITE );
	data_sec->set_addr_align( 0x4 );
	char data[] = { '\x48', '\x65', '\x6C', '\x6C', '\x6F',// “Hello, World!\n”
	'\x2C', '\x20', '\x57', '\x6F', '\x72','\x6C', '\x64', '\x21', '\x0A' };
	data_sec->set_data( data, sizeof( data ) );
	segment* data_seg = writer.segments.add();
	data_seg->set_type( PT_LOAD );
	data_seg->set_virtual_address( 0x08048020 );
	data_seg->set_physical_address( 0x08048020 );
	data_seg->set_flags( PF_W | PF_R );
	data_seg->set_align( 0x10 );
	data_seg->add_section_index( data_sec->get_index(),data_sec->get_addr_align() );
	writer.set_entry( 0x08048000 );
	writer.save( "hello_i386_32" );
}

void masm::elf::dump_pep10_file()
{
	using namespace ELFIO;
	elfio writer;
	
	writer.create( ELFCLASS32, ELFDATA2MSB );
	writer.set_os_abi( 0 );
	writer.set_type( ET_EXEC );
	writer.set_machine( 0xfe );

    // Create string table section
    section* str_sec = writer.sections.add( ".strtab" );
    str_sec->set_type( SHT_STRTAB );

    // Create string table writer
    string_section_accessor stra( str_sec );
    // Add label name
    Elf32_Word str_index = stra.add_string( "msg" );

	/*
	 * Load the user program.
	 */
	section* user_section = writer.sections.add( ".text" );
	user_section->set_type( SHT_PROGBITS );
	user_section->set_flags( SHF_ALLOC | SHF_EXECINSTR | SHF_WRITE);
	user_section->set_addr_align( 0x1 );
	std::vector<uint8_t> user_bytes = {10, 20, 30, 40, 50};
	user_section->set_data((const char*)user_bytes.data(), user_bytes.size());

	segment* user_segment = writer.segments.add();
	user_segment->set_type( PT_LOAD );
	user_segment->set_virtual_address( 0x0000);
	user_segment->set_physical_address( 0x0000);
	user_segment->set_flags( PF_X| PF_R  | PF_W);
	user_segment->set_align( 0x1 );
	user_segment->add_section_index( user_section->get_index(), user_section->get_addr_align() );

	// Create symbol table section
    section* sym_sec = writer.sections.add( ".symtab" );
    sym_sec->set_type( SHT_SYMTAB );
    sym_sec->set_info(10/*number of symbols*/ );
    sym_sec->set_addr_align( 0x4 );
    sym_sec->set_entry_size( writer.get_default_entry_size( SHT_SYMTAB ) );
    sym_sec->set_link( str_sec->get_index() );

    // Create symbol table writer
    symbol_section_accessor syma( writer, sym_sec );
    // Another way to add symbol
    syma.add_symbol( stra, "_main", 0x0000, 0, STB_LOCAL, STT_NOTYPE, 0, SHN_ABS);
	syma.add_symbol( stra, "_idiot", 0xFEFE, 0, STB_LOCAL, STT_NOTYPE, 0, SHN_ABS);

	/*
	 * Load the operating system.
	 */
	section* os_section = writer.sections.add( "os.text" );
	os_section->set_type( SHT_PROGBITS );
	os_section->set_flags( SHF_ALLOC | SHF_EXECINSTR);
	os_section->set_addr_align( 0x1 );
	std::vector<uint8_t> os_bytes = {10, 20, 30, 40, 50};
	os_section->set_data((const char*)os_bytes.data(), os_bytes.size());

	segment* os_segment = writer.segments.add();
	os_segment->set_type( PT_LOAD );
	os_segment->set_virtual_address(  0x10000-os_bytes.size());
	os_segment->set_physical_address( 0x10000-os_bytes.size());
	os_segment->set_flags( PF_X| PF_R);
	os_segment->set_align( 0x1 );
	os_segment->add_section_index( os_section->get_index(), os_section->get_addr_align() );

	// Embed OS symbol table 
    section* os_sym_sec = writer.sections.add( "os.symtab" );
    os_sym_sec->set_type( SHT_SYMTAB );
    os_sym_sec->set_info( 10 /*number of symbols*/);
    os_sym_sec->set_addr_align( 0x4 );
    os_sym_sec->set_entry_size( writer.get_default_entry_size( SHT_SYMTAB ) );
    os_sym_sec->set_link( str_sec->get_index() );

	symbol_section_accessor sym_os( writer, os_sym_sec );
    
    sym_os.add_symbol( stra, "_os_main", 0xFFFF, 0, STB_LOCAL, STT_FUNC, 0, SHN_ABS);


	/*
	 * Add Stacks
	 */

	// User stack
	section* user_stack_sec = writer.sections.add( "user.stack" );
	user_stack_sec->set_type( SHT_NOBITS );
	user_stack_sec->set_flags( SHF_ALLOC | SHF_WRITE );
	user_stack_sec->set_addr_align( 0x1 );

	segment* user_stack_seg = writer.segments.add();
	user_stack_seg->set_type( PT_LOAD );
	user_stack_seg->set_virtual_address(  0x10000-os_bytes.size() - 128 - 0x1000);
	user_stack_seg->set_physical_address( 0x10000-os_bytes.size() - 128 - 0x1000 );
	user_stack_seg->set_flags( PF_W | PF_R );
	user_stack_seg->set_align( 1 );
	user_stack_seg->set_memory_size(0x1000);
	user_stack_seg->add_section_index( user_stack_sec->get_index(),user_stack_sec->get_addr_align() );

	// OS stack
	section* os_stack_sec = writer.sections.add( "os.stack" );
	os_stack_sec->set_type( SHT_NOBITS );
	os_stack_sec->set_flags( SHF_ALLOC | SHF_WRITE );
	os_stack_sec->set_addr_align( 0x1 );
	os_stack_sec->set_size(128);

	segment* os_stack_seg = writer.segments.add();
	os_stack_seg->set_type( PT_LOAD );
	os_stack_seg->set_virtual_address( 0x10000-os_bytes.size() - 128 );
	os_stack_seg->set_physical_address( 0x10000-os_bytes.size() - 128 );
	os_stack_seg->set_flags( PF_W | PF_R );
	os_stack_seg->set_align( 1 );
	os_stack_seg->set_memory_size(128);
	os_stack_seg->add_section_index( os_stack_sec->get_index(), os_stack_sec->get_addr_align() );


	/*
	 * Address <=> line number mapping
	 */
	section* lines_sec = writer.sections.add( ".lines" );
	lines_sec->set_type( SHT_NOBITS );
	lines_sec->set_flags( 0 );
	lines_sec->set_addr_align( 0x4 );

	/*
	 * Add symbols + types for stack debugger
	 */
    section* stackdb_sym_sec = writer.sections.add( "stackdb.symbols" );
    stackdb_sym_sec->set_type( SHT_STRTAB );
    string_section_accessor strs( stackdb_sym_sec );
	auto sym_index = strs.add_string( "hello_symbol" );

	section* stackdb_types_sec = writer.sections.add( "stackdb.types" );
    stackdb_types_sec->set_type( SHT_STRTAB );
	string_section_accessor strt( stackdb_types_sec );
	auto type_index = strt.add_string( "#2d" );

	section* stackdb_text_sec = writer.sections.add( "stackdb.text" );
    stackdb_text_sec->set_type( SHT_PROGBITS );
	stackdb_text_sec->set_data( (const char*) user_bytes.data(), user_bytes.size() );

	writer.set_entry( 0xfefe );
	writer.save( "hello_pep10" );
}
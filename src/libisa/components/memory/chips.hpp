/*
 * Error thrown when a user attempts to write to readonly memory chips or
 * access a Nil chip.
 */
class bad_chip_write: public std::runtime_error{
public:
    explicit bad_chip_write(const std::string& what_arg) : runtime_error(what_arg){
    }
    explicit bad_chip_write (const char* what_arg): runtime_error(what_arg){
    }
    // Define this method out-of-line, so that this class's vtable is not in every translation unit.
    virtual ~bad_chip_write();
};

class io_aborted: public std::runtime_error{
public:
    explicit io_aborted(const std::string& what_arg) : runtime_error(what_arg){
    }
    explicit io_aborted (const char* what_arg): runtime_error(what_arg){
    }
    // Define this method out-of-line, so that this class's vtable is not in every translation unit.
    virtual ~io_aborted();
};

// Various kinds of memory chips supplied default
enum class ChipTypes {
	CONST = 0, // Memory device that is read/writable, but its contents remain 0
	NIL = 1, // Memory device that cannot be read/written.
	IDEV = 10, ODEV = 11, // Memory mapped IO devices.
	RAM = 20, ROM = 21 // Typical random access / read only memory.
};

enum class IOFlags: int {
    READ = 1<<0, 
	WRITE = 1<<1, 
	MEMORY_MAPPED = 1<<2, // Does the device have some kind of memory-mapped controller?
	CACHABLE = 1 <<3 // If true, then the memory chip is non-volatile and may be cached.

};

// Signature for a memory chip that must be matched
// addr_size_t is a type that can hold the maximum address of the system's address bus.
// For this reason, the chip reports quantities such as base address using this type.
// chip_offset_t represents the number of bytes contained withing the chip.
template<typename device_t, typename addr_size_t, typename chip_offset_t>
class AMemoryChip {
public:

	AMemoryChip(addr_size_t chip_width, addr_size_t base_address) noexcept;
	~AMemoryChip();

	// Change the baseAddress of the chip to newAddress
    void setBaseAddress(addr_size_t newAddress);

    // Number of bytes contained by this chip
    addr_size_t getSize() const noexcept;
    // Change the size of the chip at runtime, to avoid creating and deleting
    // an excessive number of chip instances.
    virtual void resize(addr_size_t newSize) noexcept;
    // Location in main memory where this chip is inserted
    addr_size_t getBaseAddress() const noexcept;

    // Returns the abilities of the chip |'ed together.
    // e.g. check if a chip is writable, (chip->getIOFunctions() & (int)IOFunctions::WRITE) == true
    virtual IOFlags get_IO_flags() const noexcept = 0;
    virtual ChipTypes get_type() const noexcept = 0;
    // Set the value of every address in the chip to 0.
    virtual void clear() noexcept = 0;


    // Read / Write functions that may generate signals or trap for IO.
    virtual bool read_byte(chip_offset_t offsetFromBase, uint8_t& output) const = 0;
    virtual bool write_byte(chip_offset_t offsetFromBase, uint8_t value) = 0;
	virtual bool get_byte(chip_offset_t offsetFromBase, uint8_t& output) const = 0;
    virtual bool set_byte(chip_offset_t offsetFromBase, uint8_t value) = 0;
protected:
	addr_size_t chip_size;
    addr_size_t baseAddress;
    // Helpers that throw a stylized error message for an out of bounds error message.

};

#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include <type_traits>
#include <tuple>

template <class T>
concept Integral = std::is_integral<T>::value;
template <class T>
concept SignedIntegral = Integral<T> && std::is_signed<T>::value;
template <class T>
concept UnsignedIntegral = Integral<T> && !SignedIntegral<T>;



namespace components::memory{


template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
[[noreturn]] void oob_read_helper(offset_t offsetFromBase);
template <typename offset_t>
	requires (UnsignedIntegral<offset_t>)
[[noreturn]] void oob_write_helper(offset_t offsetFromBase, uint8_t value);


template<typename offset_t>
	requires (UnsignedIntegral<offset_t>)
class storage_block
{
public:
	storage_block(offset_t max_offset);
	void clear(uint8_t fill_val=0);
    // Read / Write functions that may generate signals or trap for IO.
    uint8_t read_byte(offset_t offset) const;
	uint8_t get_byte(offset_t offset) const ;
    void write_byte(offset_t offset, uint8_t value);
    void set_byte(offset_t offset, uint8_t value);

	// Number of bytes contained by this chip
    offset_t max_offset() const noexcept;
    // Change the size of the chip at runtime, to avoid creating and deleting
    // an excessive number of chip instances.
    void resize(offset_t new_offset) noexcept;
private:
	offset_t _max_offset;
	std::vector<uint8_t> _memory;
};

template<typename offset_t>
	requires (UnsignedIntegral<offset_t>)
class storage_map
{
public:
	storage_map(offset_t max_offset, uint8_t default_value=0);
	void clear(uint8_t fill_val=0);
    // Read / Write functions that may generate signals or trap for IO.
    uint8_t read_byte(offset_t offset) const;
	uint8_t get_byte(offset_t offset) const ;
    void write_byte(offset_t offset, uint8_t value);
    void set_byte(offset_t offset, uint8_t value);

	// Number of bytes contained by this chip
    offset_t max_offset() const noexcept;
    // Change the size of the chip at runtime, to avoid creating and deleting
    // an excessive number of chip instances.
    void resize(offset_t new_offset) noexcept;
private:
	offset_t _max_offset;
	uint8_t _default;
	std::map<offset_t, uint8_t> _memory;
};

template<typename addr_t>
	requires (UnsignedIntegral<addr_t>)
struct memory_span
{
	std::tuple<addr_t, addr_t> span;
	std::vector<uint8_t> value;
};

template<typename offset_t>
	requires (UnsignedIntegral<offset_t>)
class storage_range
{
public:
	storage_range(offset_t max_offset, uint8_t default_value=0);
	void clear(uint8_t fill_val=0);
    // Read / Write functions that may generate signals or trap for IO.
    uint8_t read_byte(offset_t offset) const;
	uint8_t get_byte(offset_t offset) const ;
    void write_byte(offset_t offset, uint8_t value);
    void set_byte(offset_t offset, uint8_t value);

	// Number of bytes contained by this chip
    offset_t max_offset() const noexcept;
    // Change the size of the chip at runtime, to avoid creating and deleting
    // an excessive number of chip instances.
    void resize(offset_t new_offset) noexcept;
private:
	offset_t _max_offset;
	uint8_t _default;
	std::vector<memory_span<offset_t> > _memory;
};

}; // End namespace components::memory

#include "storage.tpp"
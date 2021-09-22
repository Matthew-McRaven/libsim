#pragma once

#include <cstdint>

#include <cereal/types/vector.hpp>

namespace masm::elf
{
	// Struct to represent an address span to listing line bijection.
	// It is designed to be serialized easily using ceral.
	// Take care that you pass the right address type, or parsing will probably implode.
	template <typename addr_size_t>
	struct AddressLineRegion
	{
		addr_size_t start_address, end_address;
		std::size_t line_number;
		bool contains_address(addr_size_t addr) const {
			return start_address <= addr && addr <= end_address;
		}
		bool contains_line(std::size_t line) const {
			return line_number == line;
		}
	private:
		friend class cereal::access;
		template <class Archive>
		void serialize( Archive & ar )
		{
			ar(start_address, end_address, line_number);
		}
	};

	// Struct to represent a collection of address<=>line mappings, since I can't serialize a vector directly.
	// It is designed to be serialized easily using ceral.
	// Take care that you pass the right address type, or parsing will probably implode.
	template <typename addr_size_t>
	struct AddressLineRegionVector
	{
		std::vector<AddressLineRegion<addr_size_t>> vec;
		/*
		 * The API really should return an iterator rather than bool+optional<size_t>.
		 * However, in the one place that it is used, this pattern would lead to ugly code.
		 * So, this ugly API design will stay for now.
		 */
		bool contains_address(addr_size_t addr) const {
			for(const auto& item : vec) 
				if(item.contains_address(addr)) return true;
			return false;
		}
		bool contains_line(std::size_t line) const {
			for(const auto& item : vec) 
				if(item.contains_line(line)) return true;
			return false;
		}

		// Must return optional, since there is no gaurentee addresses will be contained within vector, and I don't want
		// contains(x) == true to be a precondition of this function working correctly. 
		std::optional<std::size_t> line(addr_size_t addr) const {
			for(const auto& item : vec) 
				if(item.contains_address(addr)) return item.line_number;
			return std::nullopt;
		}
		// Returns base (starting) address for corresponding line if present.
		std::optional<addr_size_t> address(std::size_t line) const {
			for(const auto& item : vec) 
				if(item.contains_line(line)) return item.start_address;
			return std::nullopt;
		}
	private:
		friend class cereal::access;
		template <class Archive>
		void serialize( Archive & ar )
		{
			ar(vec);
		}
	};
}
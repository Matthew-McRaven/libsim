#pragma once

#include <map>
#include <tuple>
#include <vector>
#include "components/memory/historian.hpp"
namespace isa {

template<typename reg_name_t, typename reg_size_t, 
	typename csr_name_t, typename csr_size_t,
	typename addr_size_t>
struct delta_timestep 
{
	std::map<reg_name_t, std::tuple<reg_size_t,reg_size_t> > delta_reg;
	std::map<csr_name_t, std::tuple<csr_size_t,csr_size_t> > delta_csr;
	std::vector<components::memory::delta_memory<addr_size_t> > delta_mem;
	uint64_t timestep;
};
};
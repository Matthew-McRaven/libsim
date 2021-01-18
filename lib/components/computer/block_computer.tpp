#include <iostream>
#include <concepts>

template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
block_computer<isa_proc_t, regbank_t, mem_device_t>::block_computer(regbank_t regbank, mem_device_t memory): 
	regbank(regbank), memory(memory), proc({})
{
	_iface = {};
	_iface.begin_instruction = [](){};
	_iface.end_instruction = [](){};
	_iface.begin_transaction = [](){};
	_iface.end_transaction = [](){};

	// Wrap all member functions for memory inside a lambda, so `this` can be passed.
	auto read_mem_lambda = [this](addr_size_t addr, bool d) {return this->memory.read_byte(addr, d);};
	_iface.read_mem_byte = std::function<uint8_t(addr_size_t, bool)>(read_mem_lambda);
	auto write_mem_lambda = [this](addr_size_t addr, uint8_t val, bool d){this->memory.write_byte(addr, val, d);};
	_iface.write_mem_byte = std::function<void(addr_size_t, uint8_t, bool)>(write_mem_lambda);
	auto clear_mem_lambda = [this](){this->clear_memory();};
	_iface.clear_mem = std::function<void()>(clear_mem_lambda);

	// Wrap all member functions for registers inside a lambda, so `this` can be passed.
	auto read_reg_lambda = [this](uint8_t addr){return this->regbank.read_reg(addr);};
	_iface.read_reg = std::function<reg_size_t(uint8_t)>(read_reg_lambda);
	auto write_reg_lambda = [this](uint8_t addr, reg_size_t val){this->regbank.write_reg(addr, val);};
	_iface.write_reg = std::function<void(uint8_t, reg_size_t)>(write_reg_lambda);
	auto clear_reg_lambda = [this](){this->regbank.clear_regs();};
	_iface.clear_regs = std::function<void()>(clear_reg_lambda);

	// Wrap all member functions for csrs inside a lambda, so `this` can be passed.
	auto read_csr_lambda = [this](uint8_t addr){return this->regbank.read_csr(addr);};
	_iface.read_csr = std::function<csr_size_t(uint8_t)>(read_csr_lambda);
	auto write_csr_lambda = [this](uint8_t addr, csr_size_t val){this->regbank.write_csr(addr, val);};
	_iface.write_csr = std::function<void(uint8_t, csr_size_t)>(write_csr_lambda);
	auto clear_csr_lambda = [this](){this->regbank.clear_csrs();};
	_iface.clear_csrs = std::function<void()>(clear_csr_lambda);

	// Re-init processor with correct interface.
	proc = isa_proc_t(_iface);
}
template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
block_computer<isa_proc_t, regbank_t, mem_device_t>::sim_interface_t block_computer<isa_proc_t, regbank_t, mem_device_t>::sim_interface() {
	block_computer<isa_proc_t, regbank_t, mem_device_t>::sim_interface_t ret;

	// Wrap all member functions for memory inside a lambda, so `this` can be passed.
	auto read_mem_lambda = [this](addr_size_t addr, bool d) {return this->memory.read_byte(addr, d);};
	ret.read_mem_byte = std::function<uint8_t(addr_size_t, bool)>(read_mem_lambda);
	auto write_mem_lambda = [this](addr_size_t addr, uint8_t val, bool d){this->memory.write_byte(addr, val, d);};
	ret.write_mem_byte = std::function<void(addr_size_t, uint8_t, bool)>(write_mem_lambda);
	auto clear_mem_lambda = [this](){this->clear_memory();};
	ret.clear_mem = std::function<void()>(clear_mem_lambda);

	// Wrap all member functions for registers inside a lambda, so `this` can be passed.
	auto read_reg_lambda = [this](uint8_t addr){return this->regbank.read_reg(addr);};
	ret.read_reg = std::function<reg_size_t(uint8_t)>(read_reg_lambda);
	auto write_reg_lambda = [this](uint8_t addr, reg_size_t val){this->regbank.write_reg(addr, val);};
	ret.write_reg = std::function<void(uint8_t, reg_size_t)>(write_reg_lambda);
	auto clear_reg_lambda = [this](){this->regbank.clear_regs();};
	ret.clear_regs = std::function<void()>(clear_reg_lambda);

		// Wrap all member functions for csrs inside a lambda, so `this` can be passed.
	auto read_csr_lambda = [this](uint8_t addr){return this->regbank.read_csr(addr);};
	ret.read_csr = std::function<csr_size_t(uint8_t)>(read_csr_lambda);
	auto write_csr_lambda = [this](uint8_t addr, csr_size_t val){this->regbank.write_csr(addr, val);};
	ret.write_csr = std::function<void(uint8_t, csr_size_t)>(write_csr_lambda);
	auto clear_csr_lambda = [this](){this->regbank.clear_csrs();};
	ret.clear_csrs = std::function<void()>(clear_csr_lambda);



	ret.step = std::function<delta_t()> ([this](){return step();});
	ret.get_delta = std::function<delta_t()> ([this](){return get_delta();});
	auto apply_lambda = [this](delta_t v){return apply(v);};
	ret.apply = std::function<delta_t(delta_t)> (apply_lambda);

	ret.add_code_breakpoint = [](addr_size_t){};
	ret.remove_code_breakpoint = [](addr_size_t){};
	ret.clear_code_breakpoints = [](){};


}

template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
block_computer<isa_proc_t, regbank_t, mem_device_t>::delta_t block_computer<isa_proc_t, regbank_t, mem_device_t>::get_delta() const
{
	constexpr bool reg_delta = requires(const regbank_t& t) {
        t.get_reg_delta();
    };
	constexpr bool csr_delta = requires(const regbank_t& t) {
        t.get_csr_delta();
    };
	constexpr bool memory_delta = requires(const mem_device_t& t) {
        t.get_delta();
    };
	delta_t delta;

	if constexpr(reg_delta) {
		delta.delta_reg = regbank.get_reg_delta();
	}
	if constexpr(csr_delta) {
		delta.delta_csr = regbank.get_csr_delta();
	}
	else {
		std::cout<< "No register deltas" <<std::endl;
	}

	if constexpr(memory_delta) {
		delta.delta_mem = memory.get_delta();	
	}
	return delta;
}
template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
block_computer<isa_proc_t, regbank_t, mem_device_t>::delta_t block_computer<isa_proc_t, regbank_t, mem_device_t>::step()
{
	proc.step();
	auto temp_step = std::move(get_delta());
	// clear deltas.
	// Must re-init after move, state undefined.
	return temp_step;
}

template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
block_computer<isa_proc_t, regbank_t, mem_device_t>::delta_t block_computer<isa_proc_t, regbank_t, mem_device_t>::apply(block_computer::delta_t)
{
	throw std::invalid_argument("Not implemented");
}

template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
uint8_t block_computer<isa_proc_t, regbank_t, mem_device_t>::read_mem_byte(addr_size_t addr, bool d) const
{
	return memory.read_byte(addr, d);
}

template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
void block_computer<isa_proc_t, regbank_t, mem_device_t>::write_mem_byte(addr_size_t addr, uint8_t value, bool d)
{
	memory.write_byte(addr, value, d);
}

template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
void block_computer<isa_proc_t, regbank_t, mem_device_t>::clear_memory()
{
	return memory.clear();
}

template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
block_computer<isa_proc_t, regbank_t, mem_device_t>::reg_size_t block_computer<isa_proc_t, regbank_t, mem_device_t>::read_reg(uint8_t addr) const
{
	return regbank.read_reg(addr);
}

template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
void block_computer<isa_proc_t, regbank_t, mem_device_t>::write_reg(uint8_t addr, reg_size_t value)
{
	auto reg = static_cast<isa_proc_t::Registers>(addr);

	regbank.write_reg(addr, value);
}

template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
void block_computer<isa_proc_t, regbank_t, mem_device_t>::clear_regs()
{
	regbank.clear_regs();
}

template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
block_computer<isa_proc_t, regbank_t, mem_device_t>::csr_size_t block_computer<isa_proc_t, regbank_t, mem_device_t>::read_csr(uint8_t addr) const
{
	return regbank.read_csr(addr);
}

template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
void block_computer<isa_proc_t, regbank_t, mem_device_t>::write_csr(uint8_t addr, csr_size_t value)
{
	auto csr = static_cast<isa_proc_t::CSR>(addr);
	regbank.write_csr(addr, value);
}

template <typename isa_proc_t, typename regbank_t, typename mem_device_t>
void block_computer<isa_proc_t, regbank_t, mem_device_t>::clear_csrs()
{
	regbank.clear_csrs();
}

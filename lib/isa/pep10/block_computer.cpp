#include "isa/pep10/block_computer.hpp"

#include <iostream>

using namespace isa::pep10;

block_computer::block_computer(): proc({})
{
	_iface = {};
	_iface.begin_instruction = [](){};
	_iface.end_instruction = [](){};
	_iface.begin_transaction = [](){};
	_iface.end_transaction = [](){};

	// Wrap all member functions for memory inside a lambda, so `this` can be passed.
	auto read_mem_lambda = [this](uint16_t addr, bool) {return this->memory[addr];};
	_iface.read_mem_byte = std::function<uint8_t(uint16_t, bool)>(read_mem_lambda);
	auto write_mem_lambda = [this](uint16_t addr, uint8_t val, bool){this->memory[addr] = val;};
	_iface.write_mem_byte = std::function<void(uint16_t, uint8_t, bool)>(write_mem_lambda);
	auto clear_mem_lambda = [this](){this->clear_memory();};
	_iface.clear_mem = std::function<void()>(clear_mem_lambda);

	// Wrap all member functions for registers inside a lambda, so `this` can be passed.
	auto read_reg_lambda = [this](uint8_t addr){return this->regbank[addr];};
	_iface.read_reg = std::function<uint16_t(uint8_t)>(read_reg_lambda);
	auto write_reg_lambda = [this](uint8_t addr, uint16_t val){this->regbank[addr] = val;};
	_iface.write_reg = std::function<void(uint8_t, uint16_t)>(write_reg_lambda);
	auto clear_reg_lambda = [this](){this->clear_regs();};
	_iface.clear_mem = std::function<void()>(clear_reg_lambda);

	// Wrap all member functions for csrs inside a lambda, so `this` can be passed.
	auto read_csr_lambda = [this](uint8_t addr){return this->NZVC[addr];};
	_iface.read_csr = std::function<bool(uint8_t)>(read_csr_lambda);
	auto write_csr_lambda = [this](uint8_t addr, bool val){this->NZVC[addr] = val;};
	_iface.write_csr = std::function<void(uint8_t, bool)>(write_csr_lambda);
	auto clear_csr_lambda = [this](){this->clear_csrs();};
	_iface.clear_mem = std::function<void()>(clear_csr_lambda);

	// Re-init processor with correct interface.
	proc = isa::pep10::isa_processor(_iface);
}

block_computer::sim_interface_t block_computer::sim_interface() {
	sim_interface_t ret;

	// Wrap all member functions for memory inside a lambda, so `this` can be passed.
	auto read_mem_lambda = [this](uint16_t addr, bool) {return this->memory[addr];};
	ret.read_mem_byte = std::function<uint8_t(uint16_t, bool)>(read_mem_lambda);
	auto write_mem_lambda = [this](uint16_t addr, uint8_t val, bool){this->memory[addr] = val;};
	ret.write_mem_byte = std::function<void(uint16_t, uint8_t, bool)>(write_mem_lambda);
	auto clear_mem_lambda = [this](){this->clear_memory();};
	ret.clear_mem = std::function<void()>(clear_mem_lambda);

	// Wrap all member functions for registers inside a lambda, so `this` can be passed.
	auto read_reg_lambda = [this](uint8_t addr){return this->regbank[addr];};
	ret.read_reg = std::function<uint16_t(uint8_t)>(read_reg_lambda);
	auto write_reg_lambda = [this](uint8_t addr, uint16_t val){this->regbank[addr] = val;};
	ret.write_reg = std::function<void(uint8_t, uint16_t)>(write_reg_lambda);
	auto clear_reg_lambda = [this](){this->clear_regs();};
	ret.clear_mem = std::function<void()>(clear_reg_lambda);

	// Wrap all member functions for csrs inside a lambda, so `this` can be passed.
	auto read_csr_lambda = [this](uint8_t addr){return this->NZVC[addr];};
	ret.read_csr = std::function<bool(uint8_t)>(read_csr_lambda);
	auto write_csr_lambda = [this](uint8_t addr, bool val){this->NZVC[addr] = val;};
	ret.write_csr = std::function<void(uint8_t, bool)>(write_csr_lambda);
	auto clear_csr_lambda = [this](){this->clear_csrs();};
	ret.clear_mem = std::function<void()>(clear_csr_lambda);

	ret.step = std::function<delta_t()> ([this](){return step();});
	ret.get_delta = std::function<delta_t()> ([this](){return get_delta();});
	auto apply_lambda = [this](delta_t v){return apply(v);};
	ret.apply = std::function<delta_t(delta_t)> (apply_lambda);

	ret.add_code_breakpoint = [](uint16_t){};
	ret.remove_code_breakpoint = [](uint16_t){};
	ret.clear_code_breakpoints = [](){};
}
block_computer::delta_t block_computer::step()
{
	delta_t temp_step;
	proc.step();
	temp_step = std::move(current_step);
	// Must re-init after move, state undefined.
	current_step = {};
	return temp_step;
}
block_computer::delta_t block_computer::apply(block_computer::delta_t)
{
	throw std::invalid_argument("Not implemented");
}

uint8_t block_computer::read_mem_byte(uint16_t addr, bool) const
{
	return this->memory[addr];
}

void block_computer::write_mem_byte(uint16_t addr, uint8_t value, bool)
{
	bool inserted = false, check_merge=false;
	// Attempt to modify an existing delta in-place if possible.
	for(auto& delta_mem : current_step.memory_delta) {
		if (auto dist = addr - std::get<0>(delta_mem.addr_span);
			dist <= std::get<1>(delta_mem.addr_span) - std::get<0>(delta_mem.addr_span)) {
				delta_mem.new_value[dist] = value;
				inserted = true;
		}
		// Only extend uperwards, depend on merg step to combine downward.
		else if(addr == std::get<1>(delta_mem.addr_span) + 1) {
			delta_mem.addr_span = {std::get<0>(delta_mem.addr_span), addr};
			delta_mem.new_value.push_back(value);
			delta_mem.old_value.push_back(memory[addr]);
			inserted = true;
			check_merge = true;
		}
	}
	// Couldn't find an exisiting delta to modify, so must create new delta.
	if(!inserted) {
		current_step.memory_delta.push_back({{addr,addr},{memory[addr]},{value}});
		check_merge = true;
	}
	// Check if we can merge any two deltas into a single delta.
	// Since we apply this step anytime our data changes, there is at most one change,
	// which means ranges may now be connected, but no range will overlap (i.e. have 
	// duplicate data elements).
	if(check_merge) {
		
		using _t = delta_memory<uint16_t>;
		// Will compare on first key of tuple. Sufficient behavior because there should be no overlap between segments.
		auto compare = [](_t lhs, _t rhs){return lhs.addr_span<rhs.addr_span;};
		std::sort(current_step.memory_delta.begin(), current_step.memory_delta.end(), compare);

		// Allow endpoint to be re-computed as we remove elements
		for(int it=0; it<current_step.memory_delta.size()-1; it++) {
			// Since our deltas are sorted, only next could be adjacent to
			// current on current's right/upper bound. Since we iterate from 0>0xffff,
			// no need to compare LHS.
			auto cur = current_step.memory_delta[it];
			auto next = current_step.memory_delta[it+1];
			// Delta only borders in upperbound and lower bound are adjacent integers.
			if(std::get<1>(cur.addr_span)+1 == std::get<0>(next.addr_span)) {
				// Merge current+next into current.
				cur.addr_span = {std::get<0>(cur.addr_span), std::get<1>(next.addr_span)};
				cur.old_value.insert( cur.old_value.end(), next.old_value.begin(), next.old_value.end() );
				cur.new_value.insert( cur.new_value.end(), next.new_value.begin(), next.new_value.end() );
				// Remove next delta, since it has been merged in.
				current_step.memory_delta.erase(current_step.memory_delta.begin()+it+1);
			}
		}


	}

	this->memory[addr] = value;
}
void block_computer::clear_memory()
{
	current_step.memory_delta.clear();
	memory.fill({0});
}

uint16_t block_computer::read_reg(uint8_t addr) const
{
	return regbank[addr];
}

void block_computer::write_reg(uint8_t addr, uint16_t value)
{
	auto reg = static_cast<isa::pep10::Registers>(addr);


	// If reg has been updated this step, update existing entry,
	// since we must preserve original value. Otherwise, create a new entry.
	auto entry = current_step.delta_reg.find(reg);
	if(entry != current_step.delta_reg.end()) {
		// Fetches the true original value in case of multiple modifications.
		entry->second = {std::get<0>(entry->second), value};
	}
	else {
		current_step.delta_reg[reg] = {read_reg(addr), value};
	}

	regbank[addr] = value;
}

void block_computer::clear_regs()
{
	current_step.delta_reg.clear();
	regbank.fill({0});
}

uint16_t block_computer::read_csr(uint8_t addr) const
{
	return NZVC[addr];
}

void block_computer::write_csr(uint8_t addr, bool value)
{
	auto csr = static_cast<isa::pep10::CSR>(addr);

	// If CSR has been updated this step, update existing entry,
	// since we must preserve original value. Otherwise, create a new entry.
	auto entry = current_step.delta_csr.find(csr);
	if(entry != current_step.delta_csr.end()) {
		// Fetches the true original value in case of multiple modifications.
		entry->second = {std::get<0>(entry->second), value};
	}
	else {
		current_step.delta_csr[csr] = {read_csr(addr), value};
	}
	NZVC[addr] = value;
}

void block_computer::clear_csrs()
{
	current_step.delta_csr.clear();
	NZVC.fill({false});
}
#pragma once

#include <climits>
#include <memory>

#include "types.hpp"
// File: symbolvalue.h
/*
    The Pep/9 suite of applications (Pep9, Pep9CPU, Pep9Micro) are
    simulators for the Pep/9 virtual machine, and allow users to
    create, simulate, and debug across various levels of abstraction.

    Copyright (C) 2018 J. Stanley Warford & Matthew McRaven, Pepperdine University

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

namespace symbol {
template <typename value_t>
class entry;

/*
 * A symbol's value type can be EMPTY (i.e the symbol is undefined), or an address (i.e. the symbol is singly defined, so it describes a ADDRESS in a program).
 * A symbol's value type is not meaninful in the case of a multiply defined symbol, so no special type is required for it.
 */

/*
 * Abstract base class defining the properties of a symbolic value.
 */
template <typename value_t=uint16_t>
class abstract_value
{
private:
public:
    abstract_value() = default;
	virtual ~abstract_value() = default;
	virtual value_t value() const = 0;
    virtual type type() const = 0;
    // Number of bytes needed to store the value of the symbol.
    // Can be variable, especially if the symbol *is a* string.
    virtual size_t size() const {return 2;} 
    virtual bool relocatable() const {return false;}
};

/**
 * A symbol value containing an indefinite value.
 */
template <typename value_t=uint16_t>
class value_empty :
public abstract_value<value_t>
{
public:
    value_empty();
    virtual ~value_empty() override = default;
    value_t value() const override;
    symbol::type type() const override;
};


template <typename value_t=uint16_t>
class value_deleted : 
public value_empty<value_t>
{
public:
    value_deleted() = default;
    virtual ~value_deleted() override = default;
    symbol::type type() const override;
};

/**
 * A symbol value containing an constant numeric value.
 */
template <typename value_t=uint16_t>
class value_const :
public abstract_value<value_t>
{
    value_t value_;
public:
    explicit value_const(value_t value);
    virtual ~value_const() override = default;
    value_t value() const override;
    void set_value(value_t);
    symbol::type type() const override;
};

/**
 * A symbol value representing an address of a line of code.
 * The effective address (and thus the value) is base + offset.
 *
 * Having a seperate offset parameter allows for easy relocation of programs,
 * which is necessary when compiling the Pep9OS.
 */
template <typename value_t=uint16_t>
class value_location :
public abstract_value<value_t>
{
    value_t base_, offset_;
public:
    explicit value_location(value_t base, value_t offset);
    virtual ~value_location() override = default; 

    // Necessary to relocate symbols while linking.
    void add_to_offset(value_t value);
    void set_offset(value_t value);

    value_t offset() const;
    value_t base() const;

    // Inherited via value.
    virtual value_t value() const override;
    symbol::type type() const override;
    bool relocatable() const override;
};

/*
 * A symbol value pointing to a symbol entry in a different table.
 *
 * This "pointer" value is necessary to implement .EXPORT statements in
 * the PEP10 operating system.
 */
template <typename value_t=uint16_t>
class value_pointer :
public abstract_value<value_t>
{
public:
    explicit value_pointer(std::shared_ptr<const entry<value_t>>);
    ~value_pointer() override;
    // Inherited via AbstractSymbolValue
    value_t value() const override;
    symbol::type type() const override;
    // Since we are pointing to a symbol in another table,
    // don't allow the symbol to be modified here.
    std::shared_ptr<const entry<value_t>> symbol_pointer;

};
}; // end namespace symbol

#include "value.tpp"
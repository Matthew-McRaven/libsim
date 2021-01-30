#pragma once

#include <climits>
#include <memory>
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
template <typename AT>
class SymbolEntry;

/*
 * A symbol's value type can be EMPTY (i.e the symbol is undefined), or an address (i.e. the symbol is singly defined, so it describes a ADDRESS in a program).
 * A symbol's value type is not meaninful in the case of a multiply defined symbol, so no special type is required for it.
 */
enum class SymbolType
{
    EMPTY, ADDRESS, NUMERIC_CONSTANT, EXTERNAL
};

/*
 * Abstract base class defining the properties of a symbolic value.
 */
template <typename value_t=uint16_t>
class AbstractSymbolValue
{
private:
public:
    AbstractSymbolValue();
	virtual ~AbstractSymbolValue();
	virtual value_t getValue() const = 0;
    virtual SymbolType getSymbolType() const = 0;
    virtual bool canRelocate() const {return false;}
};

/*
 * A symbol value representing the value contained by an undefined symbol.
 */
template <typename value_t=uint16_t>
class SymbolValueEmpty :
public AbstractSymbolValue<value_t>
{
public:
    SymbolValueEmpty();
    virtual ~SymbolValueEmpty() override;
    
    // Inherited via AbstractSymbolValue
    virtual value_t getValue() const override;
    virtual SymbolType getSymbolType() const override;
};

/*
 * A symbol value representing an constant numeric value.
 */
template <typename value_t=uint16_t>
class SymbolValueNumeric :
public AbstractSymbolValue<value_t>
{
    value_t value;
public:
    explicit SymbolValueNumeric(value_t value);
    virtual ~SymbolValueNumeric() override;
    void setValue(value_t value);
    // Inherited via AbstractSymbolValue
    virtual value_t getValue() const override;
    virtual SymbolType getSymbolType() const override;
};

/*
 * A symbol value representing an address of a line of code.
 * The effective address (and thus the value) is base + offset.
 *
 * Having a seperate offset parameter allows for easy relocation of programs,
 * which is necessary when compiling the Pep9OS.
 */
template <typename value_t=uint16_t>
class SymbolValueLocation :
public AbstractSymbolValue<value_t>
{
    value_t base, offset;
public:
    explicit SymbolValueLocation(value_t base, value_t offset);
    virtual ~SymbolValueLocation() override; 
    void setBase(value_t value);
    void setOffset(value_t value);
    // Inherited via AbstractSymbolValue
    virtual value_t getValue() const override;
    virtual SymbolType getSymbolType() const override;
    virtual bool canRelocate() const override;
    value_t getOffset() const;
    value_t getBase() const;
};

/*
 * A symbol value pointing to a symbol entry in a different table.
 *
 * This "pointer" value is necessary to implement .EXPORT statements in
 * the PEP10 operating system.
 */
template <typename value_t=uint16_t>
class SymbolValueExternal :
public AbstractSymbolValue<value_t>
{
public:
    explicit SymbolValueExternal(std::shared_ptr<const SymbolEntry<value_t>>);
    ~SymbolValueExternal() override;
    // Inherited via AbstractSymbolValue
    value_t getValue() const override;
    SymbolType getSymbolType() const override;
    bool canRelocate() const override;
    // Since we are pointing to a symbol in another table,
    // don't allow the symbol to be modified here.
    std::shared_ptr<const SymbolEntry<value_t>> getSymbolValue();
private:
    std::shared_ptr<const SymbolEntry<value_t>> symbol;

};
}; // end namespace symbol

#include "value.tpp"
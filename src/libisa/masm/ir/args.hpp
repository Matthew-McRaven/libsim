#pragma once

#include <climits>
#include <string>

#include "symbol/entry.hpp"

namespace masm::ir {

// Abstract Argument class
template <typename address_size_t>
class lir_argument
{
public:
    virtual ~lir_argument() = default;
    virtual address_size_t argument_value() const = 0;
    virtual std::string argument_string() const = 0;
};

// Concrete argument classes
template <typename address_size_t>
class char_argument: public lir_argument<address_size_t>
{
public:
    explicit char_argument(std::string char_value);
    virtual ~char_argument() override = default;
    virtual address_size_t argument_value() const override;
    virtual std::string argument_string() const override;
private:
    std::string char_value_;
};

template <typename address_size_t>
class dec_argument: public lir_argument<address_size_t>
{
public:
    explicit dec_argument(address_size_t dec_value);
    virtual ~dec_argument() override = default;
    virtual address_size_t argument_value() const override;
    virtual std::string argument_string() const override;
private:
    address_size_t dec_value_;
};

template <typename address_size_t>
class unsigned_dec_argument: public lir_argument<address_size_t>
{

public:
    explicit unsigned_dec_argument(address_size_t dec_value);
    virtual ~unsigned_dec_argument() override = default;
    virtual address_size_t argument_value() const override;
    virtual std::string argument_string() const override;
private:
    address_size_t dec_value_;
};

template <typename address_size_t>
class hex_argument: public lir_argument<address_size_t>
{
public:
    explicit hex_argument(address_size_t hex_value);
    virtual ~hex_argument() override = default;
    virtual address_size_t argument_value() const override;
    virtual std::string argument_string() const override;
private:
    address_size_t hex_value_;
};

// String argument used for instructions. At most sizeof(address_size_t) bytes.
template <typename address_size_t>
class string_argument: public lir_argument<address_size_t>
{
public:
    explicit string_argument(std::string string_value);
    virtual ~string_argument() override = default;
    virtual address_size_t argument_value() const override;
    virtual std::string argument_string() const override;
private:
    std::string string_value_;
};

// Extended string argument. Does not support argument_value(), because it is
// arbitrarily lengthed.
template <typename address_size_t>
class ascii_argument: public lir_argument<address_size_t>
{
public:
    explicit ascii_argument(std::string string_value, size_t max_size = 2);
    virtual ~ascii_argument() override = default;
    virtual address_size_t argument_value() const override;
    virtual std::string argument_string() const override;
    std::vector<uint8_t> argument_bytes() const;
private:
    std::string string_value_;
};

template <typename address_size_t>
class symbol_ref_argument: public lir_argument<address_size_t>
{
public:
    explicit symbol_ref_argument(std::shared_ptr<const symbol::SymbolEntry<address_size_t> > ref_value);
    virtual ~symbol_ref_argument() override = default;
    virtual address_size_t argument_value() const override;
    virtual std::string argument_string() const override;
    std::shared_ptr<const symbol::SymbolEntry<address_size_t> > symbol_value();
private:
    std::shared_ptr<const symbol::SymbolEntry<address_size_t> > ref_value_;

};

}; // End namespace masm::ir

#include "args.tpp"
#pragma once
#include <memory>
#include <optional>

#include "masm/ir/base.hpp"
#include "masm/ir/args.hpp"

namespace asmb::pep10
{
template <typename address_size_t>
class dot_export: public masm::ir::linear_line<address_size_t>
{
public:
    dot_export();
    ~dot_export() override = default;
    dot_export(const dot_export& other);
    dot_export& operator=(dot_export other);
    std::shared_ptr<masm::ir::linear_line<address_size_t> > clone() const override;

    // linear_line interface
    std::string generate_listing_string() const override;
    std::string generate_source_string() const override;

    friend void swap(dot_export& first, dot_export& second)
    {
        using std::swap;
        swap(static_cast<masm::ir::linear_line<address_size_t>&>(first), 
            static_cast<masm::ir::linear_line<address_size_t>&>(second));
        swap(first.argument, second.argument);
    }

    std::shared_ptr<masm::ir::symbol_ref_argument<address_size_t> > argument = nullptr;
};

template <typename address_size_t>
class dot_sycall: public masm::ir::linear_line<address_size_t>
{
public:
    dot_sycall();
    ~dot_sycall() override = default;
    dot_sycall(const dot_sycall& other);
    dot_sycall& operator=(dot_sycall other);
    std::shared_ptr<masm::ir::linear_line<address_size_t> > clone() const override;

    // linear_line interface
    std::string generate_listing_string() const override;
    std::string generate_source_string() const override;

    friend void swap(dot_sycall& first, dot_sycall& second)
    {
        using std::swap;
        swap(static_cast<masm::ir::linear_line<address_size_t>&>(first), 
            static_cast<masm::ir::linear_line<address_size_t>&>(second));
        swap(first.argument, second.argument);
    }

    std::shared_ptr<masm::ir::symbol_ref_argument<address_size_t> > argument = nullptr;
};

template <typename address_size_t>
class dot_usycall: public masm::ir::linear_line<address_size_t>
{
public:
    dot_usycall();
    ~dot_usycall() override = default;
    dot_usycall(const dot_usycall& other);
    dot_usycall& operator=(dot_usycall other);
    std::shared_ptr<masm::ir::linear_line<address_size_t> > clone() const override;

    // linear_line interface
    std::string generate_listing_string() const override;
    std::string generate_source_string() const override;

    friend void swap(dot_usycall& first, dot_usycall& second)
    {
        using std::swap;
        swap(static_cast<masm::ir::linear_line<address_size_t>&>(first), 
            static_cast<masm::ir::linear_line<address_size_t>&>(second));
        swap(first.argument, second.argument);
    }

    std::shared_ptr<masm::ir::symbol_ref_argument<address_size_t> > argument = nullptr;
};




}; // End namespace asmb::pep10
#include "directives.tpp"
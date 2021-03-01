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

/*
class DotExport: public AsmCode
{
    QSharedPointer<SymbolRefArgument> argument = nullptr;
public:
    DotExport() = default;
    ~DotExport() override = default;
    DotExport(const DotExport& other);
    DotExport& operator=(DotExport other);
    AsmCode *cloneAsmCode() const override;
    void appendObjectCode(QList<int> &objectCode) const override;

    // AsmCode interface
    QString getAssemblerListing() const override;
    QString getAssemblerSource() const override;
    quint16 objectCodeLength() const override;

    bool hasSymbolicOperand() const override;
    QSharedPointer<const SymbolEntry> getSymbolicOperand() const override;
    QSharedPointer<SymbolRefArgument> getArgument() const;
    void setArgument(QSharedPointer<SymbolRefArgument>);

    friend void swap(DotExport& first, DotExport& second)
    {
        using std::swap;
        swap(static_cast<AsmCode&>(first), static_cast<AsmCode&>(second));
        swap(first.argument, second.argument);
    }
};

class DotSycall: public AsmCode
{
    QSharedPointer<SymbolRefArgument> argument = nullptr;
public:
    DotSycall() = default;
    ~DotSycall() override = default;
    DotSycall(const DotSycall& other);
    DotSycall& operator=(DotSycall other);
    AsmCode *cloneAsmCode() const override;
    void appendObjectCode(QList<int> &objectCode) const override;

    // AsmCode interface
    QString getAssemblerListing() const override;
    QString getAssemblerSource() const override;
    quint16 objectCodeLength() const override;

    bool hasSymbolicOperand() const override;
    QSharedPointer<const SymbolEntry> getSymbolicOperand() const override;
    QSharedPointer<SymbolRefArgument> getArgument() const;
    void setArgument(QSharedPointer<SymbolRefArgument>);

    friend void swap(DotSycall& first, DotSycall& second)
    {
        using std::swap;
        swap(static_cast<AsmCode&>(first), static_cast<AsmCode&>(second));
        swap(first.argument, second.argument);
    }
};

class DotUSycall: public AsmCode
{
    QSharedPointer<SymbolRefArgument> argument = nullptr;
public:
    DotUSycall() = default;
    ~DotUSycall() override = default;
    DotUSycall(const DotUSycall& other);
    DotUSycall& operator=(DotUSycall other);
    AsmCode *cloneAsmCode() const override;
    void appendObjectCode(QList<int> &objectCode) const override;

    // AsmCode interface
    virtual QString getAssemblerListing() const override;
    QString getAssemblerSource() const override;
    quint16 objectCodeLength() const override;

    bool hasSymbolicOperand() const override;
    QSharedPointer<const SymbolEntry> getSymbolicOperand() const override;
    QSharedPointer<SymbolRefArgument> getArgument() const;
    void setArgument(QSharedPointer<SymbolRefArgument>);

    friend void swap(DotUSycall& first, DotUSycall& second)
    {
        using std::swap;
        swap(static_cast<AsmCode&>(first), static_cast<AsmCode&>(second));
        swap(first.argument, second.argument);
    }
};
*/
}; // End namespace asmb::pep10
#include "directives.tpp"

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
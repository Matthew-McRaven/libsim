
#pragma once

#include <tuple>
#include <string>

struct PreprocessorResult
{
    bool succes;
    QSharedPointer<FrontEndError> error;
};

/*
 * The Macro preprocessor is responsible for discovering any macros
 * referenced in the root module of a ModuleAssemblyGraph, and
 * recursively discovering all referenced macros.
 *
 * It validates that all macro names/arguments are syntatically correct
 * (e.g. they are valid tokens) and semantically correct (macro invocations
 * do not use a macro substitution as part of their identifier or argument list).
 *
 * It treats macros as case insensitive, e.g. @AsRa4 will match against @ASRA4,
 * which is a core macro.
 */
class MacroPreprocessor
{
public:
    MacroPreprocessor( );
    virtual ~MacroPreprocessor();
	// Give the preprocessor a graph to operate on.
    // Recursively discover and validate macros.
    virtual PreprocessorResult preprocess(void* target, const ErrorHandler* handler, const MacroRegistry* registry);

protected:
    // A list of all macro invocations or
    // an error regarding macro invocation syntax.
    struct MacroExtractResult
    {
        bool syntaxError;
        ErrorInfo error;
        QList<MacroDefinition> moduleDefinitionList;
    };

    // Report correctness of macro invocation semantics.
    struct DependencyResult
    {
        bool semanticsError;
        ErrorInfo error;
    };

    // Find all macro invocations within a module and validate invocation syntax.
    ExtractResult extractMacroDefinitions(ModulePrototype& module);
    // Add module references to ModuleAssemblyGraph and validate macro semantics.
    LinkResult addModuleLinksToPrototypes(ModulePrototype& module, QList<MacroDefinition>);

    // Return true if the target graph has a cycle in it.
    std::tuple<bool, std::list<quint16>> checkForCycles();

    // Is the macro name valid?
    std::tuple<bool, QString> validateMacroName(QString macroName);
    // If any of the macro args are macro substitutions ($1, $2, etc)
    // then the argument list is invalid.
    std::tuple<bool, QString> validateMacroArgs(QStringList macroArgs);

private:
    // Keep track of the next available vertex.
    quint16 moduleIndex;
    // Create a module or return a pointer to an identical one.
    quint16 maybeCreatePrototype(QString macroName, ModuleType type);
    QSharedPointer<ModuleInstance> maybeCreateInstance(quint16 moduleIndex, QStringList args);
};

static const std::string tooManyMacros = ";ERROR: Only one macro may be referenced per line.";
static const std::string noIdentifier = ";ERROR: A @ must be followed by a string identifier.";
static const std::string noSuchMaro = ";ERROR: Referenced macro does not exist.";
static const std::string badArgCount = ";ERROR: Macro supplied wrong number of arguments.";
static const std::string noDollarInMacro = ";ERROR: Cannot use $ as part of a macro identifier.";
static const std::string invalidArg = ";ERROR: Bad argument: {}. Cannot use $ in macro argument.";
static const std::string circularInclude = ";ERROR: Circular macro inclusion detected.";
static const std::string selfRefence = ";ERROR: Macro definition invokes itself.";

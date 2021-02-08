# Assembler

Whoah, that's a lot of files! Why are they organized this way?


One change from the Pep/9 model to the Pep/10 model is that we don't want to preserve assembler state between runs.
That is, the operating system can no longer be cached.
This decision was made so that the assembler can be stateless--allowing for a prettier RESTful API.
Therefore, each "project" consists of the main source file you want to assemble (e.g., a user program) as well as the operating system.
So, a project has multiple top level files, which necesitates a more complicated architecture than was present in the Pep/9 and early Pep/10 assemblers.
# Phases
Front End -> Middle End -> Back End

Front End:

1. Macro pre-processor
	* Discover all macros recurssively.
	* Register all system calls (if in OS)
	* Perform macro substitution where applicable.
2. Instancing
   * Instance converts prototypes to instances of the prototype.

Middle End	
 
1. Tokenizer
	* This per-target phase is responsible for turning source text into tokens to be consumed by the assembler.
	* An abstract base class in `middleend` provides a stable interface, but each target must implement their own tokenizer.
2. Assembler
	* This per-target phase is responsible for turning tokens into a linear intermediate representation.
	* An abstract base class in `middleend` provides a stable interface, but each target must implement their own assembler.

Back End

# Projects
Each project is instantiated from one or two top level source files.
If only one top level source file is provided, it is assumed that source file is the operating system.
If two are provided, argument order determines which one is the OS and which one is the user program.

A module corresponds to a top-level collection of source files.
For example, the user program and operating system would each be a module.
In RISC-V, the C++ standard libary would also be a module.
Each module has a separate symbol table, with only symbols marked as .EXPORT shared between modules at inter-link time.

Each module consists of one or more sections.
By default, everything is placed in the main section "all".
Adding additional sections allows our linker to re-arrange code and data in well-formed programs.

# Macro Registry
Since the OS is not preseved between runs, system call macros are not preserved across runs.
Therefore, a clean macro registry shall be created for each project.
Appropriate macros shall be loaded by the caller prior to kicking-off

# Front end
The front end operate directly on raw text.
Internally, it operates on a DAG representing dependencies between macros.
It shall output a DAG of modules.

## Preprocessor
The preprocessor shall create a DAG of all macros that have been invoked.
When processing the operating system, it shall fill the registry with system call macros.

## Instancer.
The instancer converts a dependency DAG to a module DAG.
This will enable a 1-to-1 correspondance between source lines of code and linear IR lines of code.
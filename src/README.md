This readme is a effort to document/overview the files/code in this project.

This code tries to be thread-safe in a lock-free way by avoiding mutability and global states. For the compilation units to communicate, each shall not be doing any work, as we'll see later.

The code also tries to be modular, each compilation/decompilation step is well decoupled from each other. That means, it can be used as a framework to deal with SCM data and/or gta3scripts.

**Note:** We'll refer only to the name of the header file here, but if there's a `.cpp` file, it's likely most, if not all, the implementation is there.

**Table of Contents**  *generated with [DocToc](http://doctoc.herokuapp.com/)*

- [General Details](#)
	- [Precompiled Header (stdinc.h)](#)
	- [Platform-specific details (system.hpp)](#)
	- [ProgramContext (program.hpp)](#)
	- [Commands (commands.hpp)](#)
- [Compiler](#)
	- [1. Tokenizer and Parser (parser.hpp)](#)
		- [1.1 Tokenizer](#)
		- [1.2 Parser](#)
	- [2. Semantic Analysis (symtable.hpp)](#)
		- [2.1 Symbol Table Building](#)
		- [2.2 Symbol Table Synchronization](#)
		- [2.3 Syntax Tree Analyzes and Annotation](#)
	- [3. Intermediate Representation Generator (compiler.hpp)](#)
	- [4. Code Generator (codegen.hpp)](#)
		- [4.1. Compute Offsets](#)
		- [4.2. Generate](#)
- [Decompiler](#)
	- [1. Disassembly (disassembler.hpp)](#)
	- [2. Dummy Text Decompiler (decompiler.hpp)](#)
## General Details

Here are described stuff not strictly related to either the compiler or the decompiler.

### Precompiled Header (`stdinc.h`)

Standard libraries that are used everywhere in the code.

_Currently, this is not configured as a PCH, but if on the future we have concerns with the compilation time, it can be easily enabled._

### Platform-specific details (`system.hpp`)

Anything that needs implementation for each different platform goes here.

### ProgramContext (`program.hpp`)

This is a state object carried over all the compilation/decompilation steps. It should hold generic states, such as the `argv` options and error messages.

_Maybe this should hold `Commands` and IDE model list (to be done) in the future too._

### Commands (`commands.hpp`)

This holds the list of **immutable** commands and constants, with all its informations, as seen in `config/name/commands.xml` and `config/name/constants.xml`.

## Compiler

This section describes the compiler, its steps and where they are.

### 1. Tokenizer and Parser (`parser.hpp`)

+ **Where:** `Script::create`.

#### 1.1 Tokenizer

+ **Where:** `TokenStream::tokenize`.
+ **Input:** Source code.
+ **Output:** Tokens.

#### 1.2 Parser

+ **Where:** `SyntaxTree::compile`.
+ **Input:** Tokens.
+ **Output:** Abstract Syntax Tree.

This step makes sense of the tokens by using the language syntax.

### 2. Semantic Analysis (`symtable.hpp`)

+ **Input:** Abstract syntax tree.
+ **Output:** Annotated abstract syntax tree and a symbol table.

This step is further separated into two and a synchronization point.

#### 2.1 Symbol Table Building

+ **Where:** `SymTable::from_script`.
+ **Input:** Abstract syntax tree.
+ **Output:** Symbol table.

This step scans the symbols of a script (`SymTable::from_script`). The symbols in question are:
  + Labels
  + Variables (and scopes)
  + References to other script files (by filename)

This step additionally scans for the number of certain special commands that needs to be matched later (e.g. **SET_COLLECTABLE1_TOTAL** and **CREATE_COLLECTABLE1**).

#### 2.2 Symbol Table Synchronization

+ **Where:** `main.cpp compile(...)` and others.
+ **Input:** Many symbol tables.
+ **Output:** Single symbol table.


This step merges the symbol table read for each script in the step above into a single one (`SymTable::merge`).

It also appends a reference to all `Script`s into this final table (`SymTable::build_script_table`).

Then, it counts the amount of times some special commands appear (e.g. **SET_COLLECTABLE1_TOTAL**) and if that's correct (`SymTable::check_command_count`).

#### 2.3 Syntax Tree Analyzes and Annotation

+ **Where:** `Script::annotate_tree`.
+ **Input:** Abstract Syntax Tree.
+ **Output:** Annotated Abstract Syntax Tree.

Checks for semantic problems and annotates the syntax tree with command information, type information and other possibly useful information.

This is also the step that resolves command matching _(for commands with same name, but different opcodes depending on argument)_.

After this step, the table of unknown models can be acquired by using `Script::compute_unknown_models` in a synchronization point.

### 3. Intermediate Representation Generator (`compiler.hpp`)

+ **Where:** `CompilerContext`.
+ **Input:** Annotated Abstract Syntax Tree and a Symbol Table.
+ **Output:** `std::vector<CompiledData>`.

This step generates a vector of pseudo-instructions that can be easily parsed be tweaked or iterated by code.

### 4. Code Generator (`codegen.hpp`)

+ **Where:** `CodeGenerator`.
+ **Input:** `std::vector<CompiledData>`.
+ **Output:** SCM Bytecode, ready for the game.

We have once again other substeps.

#### 4.1. Compute Offsets

+ **Where:** `main.cpp compile(...)`.

_This step is a synchronization point._

This step completes the information in the vector of pseudo-instructions with the _actual offset_ of the labels, scripts and variables.

First it calls `CodeGenerator::compute_labels` to find the local position of labels, then `Script::compute_script_offsets` to find the absolute position of labels.

#### 4.2. Generate

+ **Where:** `CodeGenerator::generate`.

This is where `std::vector<CompiledData>` is transformed into a bunch of bytes which the game is capable of running.

## Decompiler

The decompiler is still very early, not much to talk about it.

### 1. Disassembly (`disassembler.hpp`)

+ **Where:** `Disassembler`.
+ **Input:** SCM Bytecode.
+ **Output:** `std::vector<DecompiledData> `.

This step produces a intermediate representation (vector of pseudo-instructions) so that its easier for out code to analyze the data.

### 2. Dummy Text Decompiler (`decompiler.hpp`)

+ **Where:** `DecompilerContext`.
+ **Input:** `std::vector<DecompiledData> `.
+ **Output:** `std::string`.

This is a test. It outputs a very low-level representation of the SCM data we disassembled, so low-level it cannot be recompiled back.

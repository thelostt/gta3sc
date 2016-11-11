///
/// Code Generator
///
/// The code generator is responsible for converting the intermediate representation outputted by *compiler.hpp/cpp*
/// (stored as a vector of pseudo-instructions) into the final bytecode (aka SCM/SCC/CS instructions).
///
#pragma once
#include <stdinc.h>
#include "binary_writer.hpp"

/// Converts intermediate representation (given by `CompilerContext`) into SCM bytecode.
class CodeGenerator
{
public:
    ProgramContext&                 program;
    BinaryWriter                   bw;
    const shared_ptr<const Script>  script;

private:
    std::vector<CompiledData>       compiled;

public:
    explicit CodeGenerator(shared_ptr<const Script> script_, std::vector<CompiledData>&& compiled, ProgramContext& program) :
        program(program), script(std::move(script_)), compiled(std::move(compiled))
    {
    }

    explicit CodeGenerator(CompilerContext&& context, ProgramContext& program) : // consumes the context
        CodeGenerator(std::move(context.script), std::move(context).get_data(), program)
    {}

    /// Finds the `Label::local_offsets` for all labels that are inside this script.
    ///
    /// \returns the size of this script.
    ///
    /// \warning This method is not thread-safe because it modifies states! It modifies label objects which may be
    /// in use by other code generation units.
    ///
    uint32_t compute_labels();

    /// Generates the code.
    void generate();
    
    /// Gets the resulting buffer of the generation.
    const void* buffer() const { return this->bw.buffer(); }

    /// Gets the size of the resulting buffer of the generation.
    size_t buffer_size() const { return this->bw.buffer_size(); }
};

/// Converts intermediate of pure-data things (such as the SCM header) into a bytecode.
class CodeGeneratorData
{
public:
    ProgramContext&   program;
    BinaryWriter     bw;

private:
    const CompiledScmHeader& compiled;

public:
    /// \warning reference to header must be alive as long as this object.
    explicit CodeGeneratorData(const CompiledScmHeader& compiled, ProgramContext& program) :
        program(program), compiled(compiled)
    {}

    /// Generates the data.
    void generate();

    /// Gets the resulting buffer of the generation.
    const void* buffer() const { return this->bw.buffer(); }

    /// Gets the size of the resulting buffer of the generation.
    size_t buffer_size() const { return this->bw.buffer_size(); }
};

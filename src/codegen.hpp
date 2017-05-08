///
/// Code Generator
///
/// The code generator is responsible for converting the intermediate representation outputted by *compiler.hpp/cpp*
/// (stored as a vector of pseudo-instructions) into the final bytecode (aka SCM/SCC/CS instructions).
///
#pragma once
#include <stdinc.h>
#include "binary_writer.hpp"
#include "compiler.hpp"

class CustomHeaderOATC;

/// Converts intermediate representation (given by `CompilerContext`) into SCM bytecode.
class CodeGenerator
{
public:
    ProgramContext&                 program;
    BinaryWriter                    bw;
    const shared_ptr<const Script>  script;
    const CustomHeaderOATC*         oatc; // may be null for nullopt

private:
    std::vector<CompiledData>       compiled;

public:
    explicit CodeGenerator(shared_ptr<const Script> script_, std::vector<CompiledData>&& compiled, ProgramContext& program) :
        program(program), script(std::move(script_)), compiled(std::move(compiled)), oatc(nullptr)
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

    /// Assigns an OATC lookup to this code generator.
    ///
    /// \warning reference to header must be alive as long as this object.
    ///
    /// \warning This method is not thread-safe.
    void set_oatc(const CustomHeaderOATC& oatc) { this->oatc = std::addressof(oatc); }

    /// Optimizes the IR.
    void peepholer(ProgramContext&);

    /// Generates the code.
    void generate();
    
    /// Gets the resulting buffer of the generation.
    const void* buffer() const { return this->bw.buffer(); }

    /// Gets the size of the resulting buffer of the generation.
    size_t buffer_size() const { return this->bw.buffer_size(); }

    ///
    const std::vector<CompiledData>& ir() const { return this->compiled; };
};

/// Converts intermediate of pure-data things (such as the SCM header) into a bytecode.
class CodeGeneratorData
{
public:
    using Variant = variant<CompiledScmHeader, CustomHeaderOATC>;

public:
    ProgramContext&                 program;
    BinaryWriter                    bw;
    const shared_ptr<const Script>  script;
    const size_t                    script_offset; //< The offset (relative to script->base) this data is being generated at.

private:
    const Variant& compiled;

public:
    /// \warning reference to header must be alive as long as this object.
    explicit CodeGeneratorData(shared_ptr<const Script> script, size_t script_offset, const Variant& compiled, ProgramContext& program) :
        program(program), compiled(compiled), script(std::move(script)), script_offset(script_offset)
    {}

    /// Generates the data.
    void generate();

    /// Gets the resulting buffer of the generation.
    const void* buffer() const { return this->bw.buffer(); }

    /// Gets the size of the resulting buffer of the generation.
    size_t buffer_size() const { return this->bw.buffer_size(); }
};


/// One-at-a-Time Commands Header
/// See https://gist.github.com/thelink2012/66de0884e76d281baa08b1f878d1d08e
class CustomHeaderOATC
{
public:
    /// Builds an OATC header from all the commands used in the specified generators.
    explicit CustomHeaderOATC(const std::vector<CodeGenerator*>& gens, ProgramContext& program);

    /// If this header contains the specified command, returns its (ordinal_id + starting_opcode).
    optional<uint16_t> find_opcode(const Command&) const;

    size_t compiled_size() const;
    void generate_code(CodeGeneratorData&) const;

private:
    uint16_t starting_opcode;
    std::vector<std::pair<const Command*, uint16_t>> ordinal_commands;
};

/// List of headers for a single script.
class CompiledHeaderList
{
public:
    auto begin() const  { return headers.begin(); }
    auto end() const    { return headers.end(); }

    template<typename T>
    const std::decay_t<T>& add_header(T&& obj)
    {
        this->headers.emplace_back(std::forward<T>(obj));
        return get<std::decay_t<T>>(this->headers.back());
    }

    template<typename T>
    optional<const T&> find_header() const
    {
        for(auto& head : *this)
        {
            if(is<T>(head))
                return get<T>(head);
        }
        return nullopt;
    }

    size_t compiled_size() const;

private:
    std::list<CodeGeneratorData::Variant> headers;
};

/// List of headers for a script with multiple files.
class MultiFileHeaderList
{
public:
    template<typename T>
    const std::decay_t<T>& add_header(const shared_ptr<const Script>& script, T&& obj)
    {
        return this->headers[script].add_header(std::forward<T>(obj));
    }

    template<typename T>
    optional<const T&> find_header(const shared_ptr<const Script>& script) const
    {
        if(auto list = this->script_headers(script))
            return list->find_header<T>();
        return nullopt;
    }

    optional<const CompiledHeaderList&> script_headers(const shared_ptr<const Script>& script) const
    {
        auto it = this->headers.find(script);
        if(it != this->headers.end())
            return it->second;
        return nullopt;
    }

    size_t compiled_size(const shared_ptr<const Script>& script) const
    {
        if(auto opt = this->script_headers(script))
            return opt->compiled_size();
        return 0;
    }

private:
    std::map<shared_ptr<const Script>, CompiledHeaderList> headers;
};
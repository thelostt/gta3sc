///
/// Compiler
///
/// The compiler is responsible for transforming a Annotated Syntax Tree, plus a previosly generated Symbol Table of such tree,
/// into a intermediate representation (vector of pseudo-instructions).
///
/// This representation will **later** on be used by the code generator (codegen.hpp/cpp) to build the SCM Bytecode.
///
#pragma once
#include "stdinc.h"

/// IR for end of argument list used in variadic argument commands.
struct EOAL
{
};

/// IR for half floats used in GTA III.
struct half
{
    float value;
};

/// IR for variable / array.
struct CompiledVar
{
    shared_ptr<Var>                            var;
    optional<variant<size_t, shared_ptr<Var>>> index;
};

/// IR for strings, no matter if it's a fixed size (8/16/128 bytes) or var length.
struct CompiledString
{
    enum class Type : uint8_t
    {
        Buffer8_3VC, // no datatype
        Buffer8,
        Buffer16,
        BufferVar,
        Buffer128,
    };

    Type        type;
    std::string storage;
};

/// IR for a single argument of a command.
using ArgVariant = variant<EOAL, int8_t, int16_t, int32_t, half, float, shared_ptr<Label>, CompiledVar, CompiledString>;

/// IR for a single command plus its arguments.
struct CompiledCommand
{
    uint16_t                 id;
    std::vector<ArgVariant> args;
};

/// IR for label **definitions**.
///
/// This is just a helper to find out where the labels are.
struct CompiledLabelDef
{
    shared_ptr<Label> label;

    size_t compiled_size() const
    {
        return 0;
    }
};

/// IR for a fundamental piece of compiled data. May be a label or a command.
struct CompiledData
{
    variant<CompiledLabelDef, CompiledCommand> data;

    CompiledData(CompiledCommand x)
        : data(std::move(x))
    {}

    CompiledData(shared_ptr<Label> x)
        : data(CompiledLabelDef{ std::move(x) })
    {}
};

size_t compiled_size(const ArgVariant& varg);
size_t compiled_size(const CompiledData& varg);

/// Transforms an annotated syntax tree into a intermediate representation (vector of pseudo-instructions).
struct CompilerContext
{
    // Inputs
    const Commands&                 commands;
    const shared_ptr<const Script>  script;
    const SymTable&                 symbols;

    // Output
    std::vector<CompiledData>       compiled;

    /// Note 1: The SyntaxTree of the script must have been anotated. (TODO explain how to annotate the tree?).
    /// Note 2: TODO explain how to generate a symbol table?
    CompilerContext(shared_ptr<const Script> script, const SymTable& symbols, const Commands& commands)
        : script(std::move(script)), symbols(symbols), commands(commands)
    {}

    /// Compiles everything on the Syntax Tree of the script.
    void compile()
    {
        Expects(compiled.empty());
        return compile_statements(*script->tree);
    }

    /// Finds the `Label::local_offsets` for all labels that are inside this script.
    ///
    /// \returns the size of this script.
    ///
    /// \warning This method is not thread-safe because it modifies states! It modifies label objects which may be
    /// in use by other compilation units (Well, not really, compilation units do not touch data from Label objects directly,
    /// however the code generator (CodeGenerator) does, so still be careful).
    ///
    /// TODO move to the code generator side instead of here, as the size may be bytecode dependant?
    uint32_t compute_labels() const
    {
        uint32_t offset = 0;
        for(auto& op : this->compiled)
        {
            if(is<CompiledLabelDef>(op.data))
            {
                get<CompiledLabelDef>(op.data).label->local_offset = offset;
            }
            else
            {
                offset += compiled_size(op);
            }
        }
        return offset;
    }

private:

    void compile_statements(const SyntaxTree& base)
    {
        for(auto it = base.begin(); it != base.end(); ++it)
        {
            switch((*it)->type())
            {
                case NodeType::Command:
                    compile_command(*it->get());
                    break;
                case NodeType::Label:
                    compile_label(*it->get());
                    break;
                case NodeType::Scope:
                    break;
                case NodeType::IF:
                    break;
                case NodeType::WHILE:
                    break;
                case NodeType::REPEAT:
                    break;
                case NodeType::SWITCH:
                    break;
                case NodeType::BREAK:
                    break;
                case NodeType::CONTINUE:
                    break;
                case NodeType::VAR_INT:
                    break;
                case NodeType::LVAR_INT:
                    break;
                case NodeType::VAR_FLOAT:
                    break;
                case NodeType::LVAR_FLOAT:
                    break;
                case NodeType::VAR_TEXT_LABEL:
                    break;
                case NodeType::LVAR_TEXT_LABEL:
                    break;
                case NodeType::VAR_TEXT_LABEL16:
                    break;
                case NodeType::LVAR_TEXT_LABEL16:
                    break;
                default:
                    Unreachable();
            }
        }
    }

    void compile_label(const SyntaxTree& label_node)
    {
        // XXX PERF this can (maybe) be faster if we cache the shared_ptr<Label> as a userdata in its SyntaxTree node
        // during the scan_symbols compilation step. The cons of that is that we'll turn SyntaxTree into a mutable thing.
        // So, do this only in last case if we need more performance.

        shared_ptr<Label> label = *this->symbols.find_label(label_node.child(0).text());
        this->compiled.emplace_back(std::move(label));
    }

    void compile_command(const SyntaxTree& command_node)
    {
        auto& command_name = command_node.child(0).text();
        auto  num_args = command_node.child_count() - 1;

        const Command& command = this->commands.match(command_node);
        std::vector<ArgVariant> args = get_args(command, command_node);

        this->compiled.emplace_back(CompiledCommand{ command.id, std::move(args) });
    }

    std::vector<ArgVariant> get_args(const Command& command, const SyntaxTree& command_node)
    {
        std::vector<ArgVariant> args;
        args.reserve(command_node.child_count() - 1);

        for(auto ita = command_node.begin() + 1; ita != command_node.end(); ++ita)
        {
            const SyntaxTree& arg_node = **ita;

            switch(arg_node.type())
            {
                case NodeType::Integer:
                {
                    args.emplace_back(get_int(arg_node.text()));
                    break;
                }
                case NodeType::Float:
                {
                    args.emplace_back(get_float(arg_node.text()));
                    break;
                }
                case NodeType::Array:
                {
                    // TODO
                    break;
                }
                case NodeType::Identifier:
                {
                    if(auto opt_label = this->symbols.find_label(arg_node.text()))
                    {
                        args.emplace_back(*opt_label);
                    }

                    // TODO
                    break;
                }
                case NodeType::ShortString:
                {
                    // TODO
                    break;
                }
                case NodeType::LongString:
                {
                    // TODO
                    break;
                }
            }
        }

        return args;
    }

    ArgVariant get_int(const std::string& s)
    {
        int32_t i = std::stoi(s, nullptr, 0);

        if(i >= std::numeric_limits<int8_t>::min() && i <= std::numeric_limits<int8_t>::max())
            return int8_t(i);
        else if(i >= std::numeric_limits<int16_t>::min() && i <= std::numeric_limits<int16_t>::max())
            return int16_t(i);
        else
            return int32_t(i);
    }

    ArgVariant get_float(const std::string& s)
    {
        // TODO GTA III
        return std::stof(s);
    }
};

/// Find outs the final compiled size, in bytes, for T.
///
/// This function should be overloaded/specialized for each pseudo-object generated by *compiler.hpp/cpp*.
/// By default it calls `T.compiled_size()`.
///
/// TODO move to the code generator side instead of here, as the size may be bytecode dependant?
template<typename T>
inline size_t compiled_size(const T& x)
{
    return x.compiled_size();
}

inline size_t compiled_size(const EOAL&)
{
    return 1;
}

inline size_t compiled_size(const int8_t&)
{
    return 1 + sizeof(int8_t);
}

inline size_t compiled_size(const int16_t&)
{
    return 1 + sizeof(int16_t);
}

inline size_t compiled_size(const int32_t&)
{
    return 1 + sizeof(int32_t);
}

inline size_t compiled_size(const half&)
{
    return 1 + sizeof(int16_t);
}

inline size_t compiled_size(const float&)
{
    return 1 + sizeof(float);
}

inline size_t compiled_size(const shared_ptr<Label>&)
{
    return 1 + sizeof(int32_t);
}

inline size_t compiled_size(const CompiledVar&)
{
    // TODO
    return 0;
}

inline size_t compiled_size(const CompiledString& s)
{
    switch(s.type)
    {
        case CompiledString::Type::Buffer8_3VC:
            return 8;
        case CompiledString::Type::Buffer8:
            return 1 + 8;
        case CompiledString::Type::Buffer16:
            return 1 + 16;
        case CompiledString::Type::BufferVar:
            return 1 + 1 + s.storage.size();
        case CompiledString::Type::Buffer128:
            return 128;
        default:
            Unreachable();
    }
}

inline size_t compiled_size(const ArgVariant& varg)
{
    return visit_one(varg, [](const auto& arg) { return ::compiled_size(arg); });
}

inline size_t compiled_size(const CompiledCommand& cmd)
{
    size_t size = sizeof(uint16_t);
    for(auto& a : cmd.args) size += ::compiled_size(a);
    return size;
}

inline size_t compiled_size(const CompiledData& data)
{
    return visit_one(data.data, [](const auto& data) { return ::compiled_size(data); });
}


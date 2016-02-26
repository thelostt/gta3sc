#pragma once
#include "stdinc.h"

struct half
{
    float value;
};

struct EOAL {}; // End of Argument List

struct CompiledVar
{
    shared_ptr<Var>                            var;
    optional<variant<size_t, shared_ptr<Var>>> index;
};

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

using ArgVariant = variant<EOAL, int8_t, int16_t, int32_t, half, float, shared_ptr<Label>, CompiledVar, CompiledString>;
struct CompiledData;

size_t compiled_size(const ArgVariant& varg);
size_t compiled_size(const CompiledData& varg);

struct CompiledCommand
{
    uint16_t                 id;
    std::vector<ArgVariant> args;
};

// Label definitions have a compiled size of 0, and thus need to be in another struct
// otherwise it'd return sizeof(int32_t).
struct CompiledLabelDef
{
    shared_ptr<Label> label;

    size_t compiled_size() const
    {
        return 0;
    }
};

struct CompiledData
{
    variant<CompiledLabelDef, CompiledCommand> data;

    CompiledData() = delete;

    CompiledData(CompiledCommand x)
        : data(std::move(x))
    {}

    CompiledData(shared_ptr<Label> x)
        : data(CompiledLabelDef{ std::move(x) })
    {}
};

struct CompilerContext
{
    const Commands&                 commands;
    const shared_ptr<const Script>  script;
    const SymTable&                 symbols;

    std::vector<CompiledData>       compiled;

    CompilerContext(shared_ptr<const Script> script, const SymTable& symbols, const Commands& commands)
        : script(std::move(script)), symbols(symbols), commands(commands)
    {}

    void compile()
    {
        return compile_statements(*script->tree);
    }

    // finds local_offsets for labels and returns size of this script
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
                    assert(!"");
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
            assert(!"");
            return 0;
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


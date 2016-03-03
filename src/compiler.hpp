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
    // TODO think about more guards?

    // Inputs
    const Commands&                 commands;
    const shared_ptr<const Script>  script;
    const SymTable&                 symbols;

    // Output
    std::vector<CompiledData>       compiled;

    // Helpers
    std::vector<shared_ptr<Label>> internal_labels;
    shared_ptr<Scope> current_scope;


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

    shared_ptr<Label> make_internal_label()
    {
        // TODO pass scope
        internal_labels.emplace_back(std::make_shared<Label>(nullptr, this->script));
        return internal_labels.back();
    }

    void compile_label(const SyntaxTree& label_node)
    {
        return compile_label(label_node.annotation<shared_ptr<Label>>());
    }

    void compile_label(shared_ptr<Label> label_ptr)
    {
        this->compiled.emplace_back(std::move(label_ptr));
    }

    void compile_command(const Command& command, std::vector<ArgVariant> args)
    {
        this->compiled.emplace_back(CompiledCommand{ command.id, std::move(args) });
    }

    void compile_statements(const SyntaxTree& base)
    {
        for(auto it = base.begin(); it != base.end(); ++it)
        {
            switch((*it)->type())
            {
                case NodeType::Block:
                    // group of anything, usually group of statements
                    compile_statements(*it->get());
                    break;
                case NodeType::Command:
                    compile_command(*it->get());
                    break;
                case NodeType::Label:
                    compile_label(*it->get());
                    break;
                case NodeType::Scope:
                    compile_scope(*it->get());
                    break;
                case NodeType::IF:
                    compile_if(*it->get());
                    break;
                case NodeType::WHILE:
                    compile_while(*it->get());
                    break;
                case NodeType::REPEAT:
                    // TODO
                    break;
                case NodeType::SWITCH:
                    // TODO
                    break;
                case NodeType::BREAK:
                    // TODO after SWITCH is done
                    break;
                case NodeType::CONTINUE:
                    // TODO after SWITCH is done
                    // this is specific to this compiler, be pedantic!
                    break;
                case NodeType::VAR_INT:
                case NodeType::LVAR_INT:
                case NodeType::VAR_FLOAT:
                case NodeType::LVAR_FLOAT:
                case NodeType::VAR_TEXT_LABEL:
                case NodeType::LVAR_TEXT_LABEL:
                case NodeType::VAR_TEXT_LABEL16:
                case NodeType::LVAR_TEXT_LABEL16:
                    // Nothing to do, vars already known from symbol table.
                    break;
                default:
                    Unreachable();
            }
        }
    }

    void compile_scope(const SyntaxTree& scope_node)
    {
        auto guard = make_scope_guard([this] {
            this->current_scope = nullptr;
        });

        Expects(this->current_scope == nullptr);
        this->current_scope = scope_node.annotation<shared_ptr<Scope>>();
        compile_statements(scope_node.child(0));
    }

    void compile_if(const SyntaxTree& if_node)
    {
        if(if_node.child_count() == 3) // [conds, case_true, else]
        {
            auto else_ptr = make_internal_label();
            auto end_ptr  = make_internal_label();
            compile_conditions(if_node.child(0), else_ptr);
            compile_statements(if_node.child(1));
            compile_command(this->commands.goto_(), { end_ptr });
            compile_label(else_ptr);
            compile_statements(if_node.child(2));
            compile_label(end_ptr);
        }
        else // [conds, case_true]
        {
            auto end_ptr = make_internal_label();
            compile_conditions(if_node.child(0), end_ptr);
            compile_statements(if_node.child(1));
            compile_label(end_ptr);
        }
    }

    void compile_while(const SyntaxTree& while_node)
    {
        auto beg_ptr = make_internal_label();
        auto end_ptr = make_internal_label();
        compile_label(beg_ptr);
        compile_conditions(while_node.child(0), end_ptr);
        compile_statements(while_node.child(1));
        compile_command(this->commands.goto_(), { beg_ptr });
        compile_label(end_ptr);
    }


    void compile_conditions(const SyntaxTree& conds_node, const shared_ptr<Label>& else_ptr)
    {
        auto compile_multi_andor = [this](const auto& conds_vector, size_t op)
        {
            compile_command(this->commands.andor(), { conv_int(op + conds_vector.child_count()) });
            for(auto& cond : conds_vector) compile_command(*cond);
        };

        switch(conds_node.type())
        {
            case NodeType::Command: // single condition
                compile_command(this->commands.andor(), { conv_int(0) });
                compile_command(conds_node);
                break;
            case NodeType::AND: // 1-8
                compile_multi_andor(conds_node, 0);
                break;
            case NodeType::OR: // 21-28
                compile_multi_andor(conds_node, 20);
                break;
            default:
                Unreachable();
        }

        compile_command(this->commands.goto_if_false(), { else_ptr });
    }


    void compile_command(const SyntaxTree& command_node)
    {
        const Command& command = this->commands.match(command_node, symbols, current_scope);
        std::vector<ArgVariant> args = get_args(command, command_node);
        return compile_command(command, std::move(args));
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
                    args.emplace_back(conv_int(arg_node.annotation<int32_t>()));
                    break;
                }

                case NodeType::Float:
                {
                    args.emplace_back(arg_node.annotation<float>());
                    break;
                }

                case NodeType::Array:
                {
                    // TODO
                    break;
                }

                case NodeType::Identifier:
                {
                    if( auto opt_var = arg_node.maybe_annotation<shared_ptr<Var>>() )
                    {
                        args.emplace_back(CompiledVar { *opt_var, nullopt });
                    }
                    else if( auto opt_label = arg_node.maybe_annotation<shared_ptr<Label>>() )
                    {
                        args.emplace_back(*opt_label);
                    }
                    else if( auto opt_int = arg_node.maybe_annotation<int32_t>() )
                    {
                        args.emplace_back(conv_int(*opt_int));
                    }
                    else if( auto opt_flt = arg_node.maybe_annotation<float>() )
                    {
                        args.emplace_back(*opt_flt);
                    }
                    else
                    {
                        Unreachable();
                    }
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

                default:
                    Unreachable();
            }
        }

        return args;
    }

    ArgVariant get_int(const std::string& s)
    {
        return conv_int(std::stoi(s, nullptr, 0));
    }

    ArgVariant get_float(const std::string& s)
    {
        // TODO GTA III
        return std::stof(s);
    }

    template<typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
    ArgVariant conv_int(T integral)
    {
        int32_t i = static_cast<int32_t>(integral);

        if(i >= std::numeric_limits<int8_t>::min() && i <= std::numeric_limits<int8_t>::max())
            return int8_t(i);
        else if(i >= std::numeric_limits<int16_t>::min() && i <= std::numeric_limits<int16_t>::max())
            return int16_t(i);
        else
            return int32_t(i);
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

inline size_t compiled_size(const CompiledVar& v)
{
    if(v.index == nullopt)
        return 1 + sizeof(uint16_t);
    else
        return 1 + sizeof(uint16_t) * 2 + sizeof(uint8_t) * 2;
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


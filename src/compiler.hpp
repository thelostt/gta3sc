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

/// IR for variable / array.
struct CompiledVar
{
    shared_ptr<Var>                            var;
    optional<variant<size_t, shared_ptr<Var>>> index;

    bool operator==(const CompiledVar& rhs) const
    {
        return this->var == rhs.var && this->index == rhs.index;
    }
};

/// IR for strings, no matter if it's a fixed size (8/16/128 bytes) or var length.
struct CompiledString
{
    enum class Type : uint8_t
    {
        TextLabel8,
        TextLabel16,
        String128,
        StringVar,
    };

    Type        type;
    std::string storage;
};

/// IR for a single argument of a command.
using ArgVariant = variant<EOAL, int8_t, int16_t, int32_t, float, shared_ptr<Label>, CompiledVar, CompiledString>;

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

/// IR for HEX data.
struct CompiledHex
{
    std::vector<uint8_t> data;

    size_t compiled_size() const
    {
        return data.size();
    }
};

/// IR for a fundamental piece of compiled data. May be a label or a command.
struct CompiledData
{
    variant<CompiledLabelDef, CompiledCommand, CompiledHex> data;

    CompiledData(CompiledCommand x)
        : data(std::move(x))
    {}

    CompiledData(std::vector<uint8_t> x)
        : data(CompiledHex { std::move(x) })
    {}

    CompiledData(shared_ptr<Label> x)
        : data(CompiledLabelDef{ std::move(x) })
    {}
};



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
        Expects(!script->top_label->local_offset);
        Expects(!script->start_label->local_offset);
        compile_label(script->top_label);
        compile_label(script->start_label); // TODO at MISSION_START actually
        return compile_statements(*script->tree);
    }

private:

    shared_ptr<Label> make_internal_label()
    {
        internal_labels.emplace_back(std::make_shared<Label>(this->current_scope, this->script));
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

    void compile_command(const Command& command, std::vector<ArgVariant> args, bool not_flag = false)
    {
        uint16_t id = command.id | (not_flag? uint16_t(0x8000) : uint16_t(0x0000));
        this->compiled.emplace_back(CompiledCommand{ id, std::move(args) });
    }

    void compile_statements(const SyntaxTree& base)
    {
        for(auto it = base.begin(); it != base.end(); ++it)
            compile_statement(*it->get());
    }

    void compile_statement(const SyntaxTree& node, bool not_flag = false)
    {
        switch(node.type())
        {
            case NodeType::Block:
                // group of anything, usually group of statements
                compile_statements(node);
                break;
            case NodeType::NOT:
                throw CompilerError("XXX NOT outside of a condition block");
                break;
            case NodeType::Command:
                compile_command(node, not_flag);
                break;
            case NodeType::Equal:
            case NodeType::Cast:
                compile_expr(node, not_flag);
                break;
            case NodeType::Label:
                compile_label(node);
                break;
            case NodeType::Scope:
                compile_scope(node);
                break;
            case NodeType::IF:
                compile_if(node);
                break;
            case NodeType::WHILE:
                compile_while(node);
                break;
            case NodeType::REPEAT:
                compile_repeat(node);
                break;
            case NodeType::SWITCH:
                // TODO SA
                break;
            case NodeType::BREAK:
                // TODO after SA SWITCH is done
                break;
            case NodeType::CONTINUE:
                // TODO after SA SWITCH is done
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

    void compile_repeat(const SyntaxTree& repeat_node)
    {
        // PERF if performance is needed, get_arg(var) can be cached in the stack

        auto& annotation = repeat_node.annotation<const RepeatAnnotation>();
        auto& times = repeat_node.child(0);
        auto& var = repeat_node.child(1);

        auto loop_ptr = make_internal_label();
        
        compile_command(annotation.set_var_to_zero, { get_arg(var), get_arg(*annotation.number_zero) });
        compile_label(loop_ptr);
        compile_statements(repeat_node.child(2));
        compile_command(annotation.add_var_with_one, { get_arg(var), get_arg(*annotation.number_one) });
        compile_command(annotation.is_var_geq_times, { get_arg(var), get_arg(times) });
        compile_command(this->commands.goto_if_false(), { loop_ptr });
    }

    void compile_expr(const SyntaxTree& eq_node, bool not_flag = false)
    {
        if(eq_node.child(1).maybe_annotation<std::reference_wrapper<const Command>>())
        {
            // 'a = b OP c' or 'a OP= b'

            const SyntaxTree& op_node = eq_node.child(1);

            const Command& cmd_set = eq_node.annotation<std::reference_wrapper<const Command>>();
            const Command& cmd_op  = op_node.annotation<std::reference_wrapper<const Command>>();

            auto a = get_arg(eq_node.child(0));
            auto b = get_arg(op_node.child(0));
            auto c = get_arg(op_node.child(1));

            if(!is_same_var(a, b))
                compile_command(cmd_set, { a, b }, not_flag);
            compile_command(cmd_op,  { a, c }, not_flag);
        }
        else
        {
            // 'a = b' or 'a =# b'
            const Command& cmd_set = eq_node.annotation<std::reference_wrapper<const Command>>();
            compile_command(cmd_set, { get_arg(eq_node.child(0)), get_arg(eq_node.child(1)) }, not_flag);
        }
    }

    void compile_command(const SyntaxTree& command_node, bool not_flag = false)
    {
        const Command& command = command_node.annotation<std::reference_wrapper<const Command>>();
        return compile_command(command, get_args(command, command_node), not_flag);
    }

    void compile_condition(const SyntaxTree& node, bool not_flag = false)
    {
        switch(node.type())
        {
            case NodeType::Command:
                return compile_command(node, not_flag);
            case NodeType::Equal:
            case NodeType::Greater:
            case NodeType::GreaterEqual:
                return compile_expr(node);
            case NodeType::Lesser:
            case NodeType::LesserEqual:
                return compile_expr(node, !not_flag);
            case NodeType::NOT:
                return compile_condition(node.child(0), !not_flag);
            default:
                Unreachable();
        }
    }

    void compile_conditions(const SyntaxTree& conds_node, const shared_ptr<Label>& else_ptr)
    {
        auto compile_multi_andor = [this](const auto& conds_vector, size_t op)
        {
            // TODO check amount of conditions <= 8

            compile_command(this->commands.andor(), { conv_int(op + conds_vector.child_count()) });
            for(auto& cond : conds_vector) compile_condition(*cond);
        };

        switch(conds_node.type())
        {
            // single condition
            case NodeType::Command:
            case NodeType::Equal:
            case NodeType::Greater:
            case NodeType::GreaterEqual:
            case NodeType::Lesser:
            case NodeType::LesserEqual:
                compile_command(this->commands.andor(), { conv_int(0) });
                compile_condition(conds_node);
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



    std::vector<ArgVariant> get_args(const Command& command, const SyntaxTree& command_node)
    {
        std::vector<ArgVariant> args;
        args.reserve(command_node.child_count() - 1);

        for(auto ita = command_node.begin() + 1; ita != command_node.end(); ++ita)
        {
            const SyntaxTree& arg_node = **ita;
            args.emplace_back( get_arg(arg_node) );
        }

        return args;
    }

    ArgVariant get_arg(const SyntaxTree& arg_node)
    {
        switch(arg_node.type())
        {
            case NodeType::Integer:
            {
                return conv_int(arg_node.annotation<int32_t>());
            }

            case NodeType::Float:
            {
                return arg_node.annotation<float>();
            }

            case NodeType::Array:
            {
                // TODO array SA
                break;
            }

            case NodeType::Identifier:
            {
                if(auto opt_var = arg_node.maybe_annotation<shared_ptr<Var>>())
                {
                    return CompiledVar{ *opt_var, nullopt };
                }
                else if(auto opt_label = arg_node.maybe_annotation<shared_ptr<Label>>())
                {
                    return *opt_label;
                }
                else if(auto opt_text = arg_node.maybe_annotation<std::string>())
                {
                    return CompiledString{ CompiledString::Type::TextLabel8, *opt_text };
                }
                else if(auto opt_int = arg_node.maybe_annotation<int32_t>())
                {
                    return conv_int(*opt_int);
                }
                else if(auto opt_flt = arg_node.maybe_annotation<float>())
                {
                    return *opt_flt;
                }
                else
                {
                    Unreachable();
                }
                break;
            }

            case NodeType::ShortString:
            {
                // TODO SA
                break;
            }

            case NodeType::LongString:
            {
                // TODO SA
                break;
            }

            default:
                Unreachable();
        }

        Unreachable();
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

    bool is_same_var(const ArgVariant& lhs, const ArgVariant& rhs)
    {
        if(is<CompiledVar>(lhs) && is<CompiledVar>(rhs))
        {
            return get<CompiledVar>(lhs) == get<CompiledVar>(rhs);
        }
        return false;
    }
};


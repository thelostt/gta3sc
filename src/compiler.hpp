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
#include "symtable.hpp"
#include "commands.hpp"

/// IR for end of argument list used in variadic argument commands.
struct EOAL
{
};

/// IR for variable / array.
struct CompiledVar
{
    shared_ptr<Var>                             var;
    optional<variant<int32_t, shared_ptr<Var>>> index;

    explicit CompiledVar(shared_ptr<Var> var, decltype(nullopt))
        : var(std::move(var)), index(nullopt)
    {}

    explicit CompiledVar(shared_ptr<Var> var, int32_t index)
        : var(std::move(var)), index(index)
    {}

    explicit CompiledVar(shared_ptr<Var> var, shared_ptr<Var> index)
        : var(std::move(var)), index(std::move(index))
    {}

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

// IR for SCM header
struct CompiledScmHeader
{
    enum class Version : uint8_t
    {
        Liberty,
        Miami,
        SanAndreas,
    };

    Version                               version;
    uint32_t                              size_global_vars_space; // including the 8 bytes of GOTO at the top
    std::vector<std::string>              models;
    std::vector<shared_ptr<const Script>> scripts;   // used to find size of main, size of highest mission and mission offsets
    uint32_t                              num_missions;
    uint32_t                              num_streamed;

    CompiledScmHeader(Version version, size_t size_globals,
                      std::vector<std::string> models_,
                      const std::vector<shared_ptr<Script>>& scripts) :
        version(version), size_global_vars_space(std::max(8u, size_globals)),
        models(std::move(models_)), num_missions(0), num_streamed(0)
    {
        this->scripts.reserve(scripts.size());
        for(auto& sc : scripts)
        {
            if(sc->type == ScriptType::Mission) ++this->num_missions;
            else if(sc->type == ScriptType::StreamedScript) ++this->num_streamed;
            this->scripts.emplace_back(sc);
        }
    }

    size_t compiled_size() const;
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

    struct LoopInfo
    {
        shared_ptr<Label> continue_label;   //< Where a CONTINUE should jump into (may be nullptr).
        shared_ptr<Label> break_label;      //< Where a BREAK should jump into
    };

    // Inputs
    ProgramContext&                 program;
    const Commands&                 commands;
    const shared_ptr<const Script>  script;
    const SymTable&                 symbols;

    // Output
    std::vector<CompiledData>       compiled;

    // Helpers
    std::vector<shared_ptr<Label>> internal_labels;
    shared_ptr<Scope>              current_scope;
    std::vector<LoopInfo>          loop_stack;


    /// Note 1: The SyntaxTree of the script must have been anotated. (TODO explain how to annotate the tree?).
    /// Note 2: TODO explain how to generate a symbol table?
    CompilerContext(shared_ptr<const Script> script, const SymTable& symbols, ProgramContext& program)
        : script(std::move(script)), symbols(symbols), commands(program.commands), program(program)
    {
        this->loop_stack.reserve(16);
    }

    /// Compiles everything on the Syntax Tree of the script.
    void compile()
    {
        Expects(compiled.empty());
        Expects(!script->top_label->local_offset);
        Expects(!script->start_label->local_offset);

        // Commands always required by the compiler.
        if(!commands.andor() || !commands.andor()->supported)
            program.fatal_error(nocontext, "XXX ANDOR undefined or unsupported");
        if(!commands.goto_() || !commands.goto_()->supported)
            program.fatal_error(nocontext, "XXX GOTO undefined or unsupported");
        if(!commands.goto_if_false() || !commands.goto_if_false()->supported)
            program.fatal_error(nocontext, "XXX GOTO_IF_FALSE undefined or unsupported");
        
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

        if(command.has_optional())
        {
            Ensures(args.size() == 0 || !is<EOAL>(args.back())); // ensure args doesn't contain EOAL already
            args.emplace_back(EOAL{});
        }

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
                program.error(node, "XXX NOT outside of a condition block");
                compile_statement(node.child(0), !not_flag);
                break;
            case NodeType::Command:
                compile_command(node, not_flag);
                break;
            case NodeType::MISSION_START:
                break;
            case NodeType::MISSION_END:
                compile_mission_end(node, not_flag);
                break;
            case NodeType::Greater:
            case NodeType::GreaterEqual:
            case NodeType::Lesser:
            case NodeType::LesserEqual:
                program.error(node, "XXX conditional expression outside of a condition block");
                break;
            case NodeType::Equal:
            case NodeType::Cast:
                compile_expr(node, not_flag);
                break;
            case NodeType::Increment:
            case NodeType::Decrement:
                compile_incdec(node, not_flag);
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
                compile_switch(node);
                break;
            case NodeType::BREAK:
                compile_break(node);
                break;
            case NodeType::CONTINUE:
                compile_continue(node);
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
            compile_command(*this->commands.goto_(), { end_ptr });
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

        loop_stack.emplace_back(LoopInfo { beg_ptr, end_ptr });

        compile_label(beg_ptr);
        compile_conditions(while_node.child(0), end_ptr);
        compile_statements(while_node.child(1));
        compile_command(*this->commands.goto_(), { beg_ptr });
        compile_label(end_ptr);

        loop_stack.pop_back();
    }

    void compile_repeat(const SyntaxTree& repeat_node)
    {
        // PERF if performance is needed, get_arg(var) can be cached in the stack

        auto& annotation = repeat_node.annotation<const RepeatAnnotation&>();
        auto& times = repeat_node.child(0);
        auto& var = repeat_node.child(1);

        auto continue_ptr = make_internal_label();
        auto break_ptr    = make_internal_label();
        auto loop_ptr     = make_internal_label();

        loop_stack.emplace_back(LoopInfo { continue_ptr, break_ptr });

        compile_command(annotation.set_var_to_zero, { get_arg(var), get_arg(*annotation.number_zero) });
        compile_label(loop_ptr);
        compile_statements(repeat_node.child(2));
        compile_label(continue_ptr);
        compile_command(annotation.add_var_with_one, { get_arg(var), get_arg(*annotation.number_one) });
        compile_command(annotation.is_var_geq_times, { get_arg(var), get_arg(times) });
        compile_command(*this->commands.goto_if_false(), { loop_ptr });
        compile_label(break_ptr);

        loop_stack.pop_back();
    }

    void compile_switch(const SyntaxTree& switch_node)
    {
        struct Case
        {
            int32_t                     value;
            shared_ptr<Label>           target;
            optional<const SyntaxTree&> case_node;
            optional<const SyntaxTree&> statements;     // nullopt on case_default if no DEFAULT
            optional<const Command&>    is_var_eq_int;
        };

        Case              case_default { 0, nullptr, nullopt, nullopt, nullopt };
        std::vector<Case> cases;
        std::vector<Case*>sorted_cases;

        auto opt_switch_start     = commands.switch_start();
        auto opt_switch_continued = commands.switch_continued();

        auto continue_ptr = nullptr;
        auto break_ptr = make_internal_label();

        const SyntaxTree& var = switch_node.child(0);

        cases.reserve(switch_node.child(1).child_count());
        for(auto& case_node : switch_node.child(1))
        {
            switch(case_node->type())
            {
                case NodeType::CASE:
                    cases.emplace_back(Case {
                        case_node->child(0).annotation<int32_t>(),
                        nullptr, *case_node, case_node->child(1),
                        *case_node->annotation<const SwitchCaseAnnotation&>().is_var_eq_int
                    });
                    break;
                case NodeType::DEFAULT:
                    case_default = Case { 0, nullptr, *case_node, case_node->child(0), nullopt };
                    break;
                default:
                    Unreachable();
            }
        }

        loop_stack.emplace_back(LoopInfo { continue_ptr, break_ptr });

        sorted_cases.resize(cases.size());
        {
            std::transform(cases.begin(), cases.end(), sorted_cases.begin(), std::addressof<Case>);
            std::sort(sorted_cases.begin(), sorted_cases.end(), [](const Case* a, const Case* b) {
                return a->value < b->value;
            });

            auto it = std::adjacent_find(sorted_cases.begin(), sorted_cases.end(), [](const Case* a, const Case* b) {
                return a->value == b->value;
            });

            if(it != sorted_cases.end())
            {
                program.error((**it).case_node.value(), "XXX CASE happens twice");
            }
        }

        if(cases.size() > 75)
            program.error(switch_node, "XXX SWITCH contains more than 75 cases");

        if(opt_switch_start && opt_switch_start->supported
        && opt_switch_continued && opt_switch_continued->supported)
        {
            case_default.target = make_internal_label();
            for(auto& xcase : cases) xcase.target = make_internal_label();

            for(size_t i = 0; i < sorted_cases.size(); )
            {
                std::vector<ArgVariant> args;
                args.reserve(18);

                const Command& switch_op = (i == 0? *opt_switch_start : *opt_switch_continued);
                size_t max_cases_here    = (i == 0? 7 : 9);

                if(i == 0)
                {
                    args.emplace_back(get_arg(var));
                    args.emplace_back(conv_int(sorted_cases.size()));
                    args.emplace_back(conv_int(case_default.statements != nullopt));
                    args.emplace_back(case_default.target);
                }

                for(size_t k = 0; k < max_cases_here; ++k, ++i)
                {
                    if(i < sorted_cases.size())
                    {
                        args.emplace_back(conv_int(sorted_cases[i]->value));
                        args.emplace_back(sorted_cases[i]->target);
                    }
                    else
                    {
                        args.emplace_back(conv_int(-1));
                        args.emplace_back(break_ptr);
                    }
                }

                compile_command(switch_op, std::move(args));
            }

            for(auto& xcase : cases)
            {
                compile_label(xcase.target);
                compile_statements(*xcase.statements);
            }

            compile_label(case_default.target);
            if(case_default.statements)
            {
                compile_statements(*case_default.statements);
            }

            compile_label(break_ptr);
        }
        else
        {
            shared_ptr<Label> next_ptr = make_internal_label();

            for(auto& xcase : cases)
            {
                xcase.target = next_ptr;
                compile_label(next_ptr);
                next_ptr = make_internal_label();
                compile_command(*xcase.is_var_eq_int, { get_arg(var), conv_int(xcase.value) });
                compile_command(*commands.goto_if_false(), { next_ptr });
                compile_statements(*xcase.statements);
            }

            case_default.target = next_ptr;
            compile_label(next_ptr);
            if(case_default.statements)
            {
                compile_statements(*case_default.statements);
            }

            compile_label(break_ptr);
        }

        loop_stack.pop_back();
    }

    void compile_break(const SyntaxTree& break_node)
    {
        if(!program.opt.allow_break_continue)
        {
            if(!break_node.maybe_annotation<const SwitchCaseBreakAnnotation&>())
            {
                program.error(break_node, "XXX BREAK only allowed at the end of a SWITCH CASE [-fbreak-continue]");
            }
            else
            {
                Expects(!loop_stack.empty());
                compile_command(*commands.goto_(), { loop_stack.back().break_label });
            }
            return;
        }

        for(auto it = loop_stack.rbegin(); it != loop_stack.rend(); ++it)
        {
            if(it->break_label)
            {
                if(!program.opt.allow_break_continue)
                {
                    // When allow_break_continue=false, allow the use of BREAK only within a SWITCH.
                    // Checking continue_label is a hacky way to verify if `it` is a SWITCH.
                    if(it->continue_label != nullptr || it != loop_stack.rbegin())
                    {
                        program.error(break_node, "XXX BREAK only works in SWITCH [-fbreak-continue]");
                    }
                }

                compile_command(*commands.goto_(), { it->break_label });
                return;
            }
        }

        program.error(break_node, "XXX BREAK outside of a loop");
    }

    void compile_continue(const SyntaxTree& continue_node)
    {
        if(!program.opt.allow_break_continue)
        {
            program.error(continue_node, "XXX CONTINUE not supported [-fbreak-continue]");
            return;
        }

        for(auto it = loop_stack.rbegin(); it != loop_stack.rend(); ++it)
        {
            if(it->continue_label)
            {
                compile_command(*commands.goto_(), { it->continue_label });
                return;
            }
        }

        program.error(continue_node, "XXX CONTINUE outside of a loop");
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
            {
                if(!is_same_var(a, c))
                {
                    compile_command(cmd_set, { a, b }, not_flag);
                    compile_command(cmd_op, { a, c }, not_flag);
                }
                else
                {
                    // Safe. The annotate_tree step won't allow non-commutative
                    // operations (substraction or division) to be compiled.
                    compile_command(cmd_op, { a, b }, not_flag);
                }
            }
            else
            {
                compile_command(cmd_op, { a, c }, not_flag);
            }
        }
        else
        {
            // 'a = b' or 'a =# b' or 'a > b' (and such)

            bool invert = (eq_node.type() == NodeType::Lesser || eq_node.type() == NodeType::LesserEqual);

            auto& a_node = eq_node.child(!invert? 0 : 1);
            auto& b_node = eq_node.child(!invert? 1 : 0);

            const Command& cmd_set = eq_node.annotation<std::reference_wrapper<const Command>>();
            compile_command(cmd_set, { get_arg(a_node), get_arg(b_node) }, not_flag);
        }
    }

    void compile_incdec(const SyntaxTree& op_node, bool not_flag = false)
    {
        auto& annotation = op_node.annotation<const IncDecAnnotation&>();
        auto& var = op_node.child(0);
        compile_command(annotation.op_var_with_one, { get_arg(var), get_arg(*annotation.number_one) });
    }

    void compile_command(const SyntaxTree& command_node, bool not_flag = false)
    {
        const Command& command = command_node.annotation<std::reference_wrapper<const Command>>();
        return compile_command(command, get_args(command, command_node), not_flag);
    }

    void compile_mission_end(const SyntaxTree& me_node, bool not_flag = false)
    {
        // Cannot call compile_command(me_node, not_flag) directly because we're not a actual command node with a name child.
        const Command& command = me_node.annotation<std::reference_wrapper<const Command>>();
        return compile_command(command, {}, not_flag);
    }

    void compile_condition(const SyntaxTree& node, bool not_flag = false)
    {
        // also see compile_conditions
        switch(node.type())
        {
            case NodeType::NOT:
                return compile_condition(node.child(0), !not_flag);
            case NodeType::Command:
                return compile_command(node, not_flag);
            case NodeType::Equal:
            case NodeType::Greater:
            case NodeType::GreaterEqual:
            case NodeType::Lesser:
            case NodeType::LesserEqual:
                return compile_expr(node, not_flag);
            default:
                Unreachable();
        }
    }

    void compile_conditions(const SyntaxTree& conds_node, const shared_ptr<Label>& else_ptr)
    {
        auto compile_multi_andor = [this](const auto& conds_node, size_t op)
        {
            if(conds_node.child_count() > 8)
                program.error(conds_node, "XXX more than 8 conditions");

            compile_command(*this->commands.andor(), { conv_int(op + conds_node.child_count() - 2) });
            for(auto& cond : conds_node) compile_condition(*cond);
        };

        switch(conds_node.type())
        {
            // single condition (also see compile_condition)
            case NodeType::NOT:
            case NodeType::Command:
            case NodeType::Equal:
            case NodeType::Greater:
            case NodeType::GreaterEqual:
            case NodeType::Lesser:
            case NodeType::LesserEqual:
                if (!this->program.opt.skip_single_ifs)
                    compile_command(*this->commands.andor(), { conv_int(0) });
                compile_condition(conds_node);
                break;
            case NodeType::AND: // 1-8
                compile_multi_andor(conds_node, 1);
                break;
            case NodeType::OR: // 21-28
                compile_multi_andor(conds_node, 21);
                break;
            default:
                Unreachable();
        }

        compile_command(*this->commands.goto_if_false(), { else_ptr });
    }



    std::vector<ArgVariant> get_args(const Command& command, const SyntaxTree& command_node)
    {
        Expects(command_node.child_count() >= 1); // command_name + [args...]

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
                auto opt_var = arg_node.child(0).maybe_annotation<shared_ptr<Var>>();
                if(opt_var)
                {
                    if(auto opt_int = arg_node.child(1).maybe_annotation<int32_t>())
                    {
                        return CompiledVar{ std::move(*opt_var), std::move(*opt_int) };
                    }
                    else if(auto opt_index = arg_node.child(1).maybe_annotation<shared_ptr<Var>>())
                    {
                        return CompiledVar{ std::move(*opt_var), std::move(*opt_index) };
                    }
                    else
                    {
                        Unreachable();
                    }
                }
                else
                {
                    Unreachable();
                }
                break;
            }

            case NodeType::Identifier:
            {
                if(auto opt_int = arg_node.maybe_annotation<int32_t>())
                {
                    return conv_int(*opt_int);
                }
                else if(auto opt_flt = arg_node.maybe_annotation<float>())
                {
                    return *opt_flt;
                }
                else if(auto opt_var = arg_node.maybe_annotation<shared_ptr<Var>>())
                {
                    return CompiledVar{ std::move(*opt_var), nullopt };
                }
                else if(auto opt_label = arg_node.maybe_annotation<shared_ptr<Label>>())
                {
                    auto label = std::move(*opt_label);
                    if(label->script->type == ScriptType::Mission
                    || label->script->type == ScriptType::StreamedScript)
                    {
                        if(this->script != label->script)
                        {
                            auto sckind = label->script->type == ScriptType::Mission? "mission" :
                                          label->script->type == ScriptType::StreamedScript? "streamed" : Unreachable();
                            program.error(arg_node, "Reference to {} label '{}' outside of its {} script.", sckind, arg_node.text(), sckind);
                        }
                    }
                    return label;
                }
                else if(auto opt_text = arg_node.maybe_annotation<const StringAnnotation&>())
                {
                    auto type = opt_text->is_varlen? CompiledString::Type::StringVar : CompiledString::Type::TextLabel8;
                    return CompiledString{ type, opt_text->string };
                }
                else if(auto opt_umodel = arg_node.maybe_annotation<const ModelAnnotation&>())
                {
                    assert(opt_umodel->where.expired() == false);
                    int32_t i32 = opt_umodel->where.lock()->find_model_at(opt_umodel->id);
                    return conv_int(i32);
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


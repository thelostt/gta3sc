#include "stdinc.h"
#include "commands.hpp"
#include "symtable.hpp"
#include "error.hpp"
#include "system.hpp"
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

Commands::Commands(std::multimap<std::string, Command, iless> commands_,
                   std::map<std::string, std::vector<const Command*>, iless> alternators_,
                   std::map<std::string, EntityType> entities_,
                   transparent_map<std::string, shared_ptr<Enum>> enums_)

    : commands(std::move(commands_)), alternators(std::move(alternators_)),
      enums(std::move(enums_)), entities(std::move(entities_))
{
    auto it_carpedmodel = this->enums.find("CARPEDMODEL");
    auto it_model       = this->enums.find("MODEL");

    assert(it_model != this->enums.end());
    assert(it_carpedmodel != this->enums.end());

    this->enum_models = it_model->second;
    this->enum_carpedmodels = it_carpedmodel->second;

    for(auto& pair : this->commands)
    {
        this->commands_by_id.emplace(pair.second.id, &pair.second);
    }

    this->cmd_SET_PROGRESS_TOTAL            = find_command("SET_PROGRESS_TOTAL");
    this->cmd_SET_TOTAL_NUMBER_OF_MISSIONS  = find_command("SET_TOTAL_NUMBER_OF_MISSIONS");
    this->cmd_SET_COLLECTABLE1_TOTAL        = find_command("SET_COLLECTABLE1_TOTAL");
    this->cmd_SWITCH_START                  = find_command("SWITCH_START");
    this->cmd_SWITCH_CONTINUED              = find_command("SWITCH_CONTINUED");
    this->cmd_GOSUB_FILE                    = find_command("GOSUB_FILE");
    this->cmd_LAUNCH_MISSION                = find_command("LAUNCH_MISSION");
    this->cmd_LOAD_AND_LAUNCH_MISSION_INTERNAL = find_command("LOAD_AND_LAUNCH_MISSION_INTERNAL");
    this->cmd_START_NEW_SCRIPT              = find_command("START_NEW_SCRIPT");
    this->cmd_START_NEW_STREAMED_SCRIPT     = find_command("START_NEW_STREAMED_SCRIPT");
    this->cmd_TERMINATE_THIS_SCRIPT         = find_command("TERMINATE_THIS_SCRIPT");
    this->cmd_SCRIPT_NAME                   = find_command("SCRIPT_NAME");
    this->cmd_RETURN                        = find_command("RETURN");
    this->cmd_RET                           = find_command("RET");
    this->cmd_GOTO                          = find_command("GOTO");
    this->cmd_GOTO_IF_FALSE                 = find_command("GOTO_IF_FALSE");
    this->cmd_ANDOR                         = find_command("ANDOR");
    this->cmd_SAVE_STRING_TO_DEBUG_FILE     = find_command("SAVE_STRING_TO_DEBUG_FILE");
    this->cmd_SKIP_CUTSCENE_START           = find_command("SKIP_CUTSCENE_START");
    this->cmd_SKIP_CUTSCENE_END             = find_command("SKIP_CUTSCENE_END");
    this->cmd_SKIP_CUTSCENE_START_INTERNAL  = find_command("SKIP_CUTSCENE_START_INTERNAL");
    this->alt_SET                           = find_alternator("SET");
    this->alt_CSET                          = find_alternator("CSET");
    this->alt_TERMINATE_THIS_CUSTOM_SCRIPT  = find_alternator("TERMINATE_THIS_CUSTOM_SCRIPT");
    this->alt_ADD_THING_TO_THING            = find_alternator("ADD_THING_TO_THING");
    this->alt_SUB_THING_FROM_THING          = find_alternator("SUB_THING_FROM_THING");
    this->alt_MULT_THING_BY_THING           = find_alternator("MULT_THING_BY_THING");
    this->alt_DIV_THING_BY_THING            = find_alternator("DIV_THING_BY_THING");
    this->alt_ADD_THING_TO_THING_TIMED      = find_alternator("ADD_THING_TO_THING_TIMED");
    this->alt_SUB_THING_FROM_THING_TIMED    = find_alternator("SUB_THING_FROM_THING_TIMED");
    this->alt_IS_THING_EQUAL_TO_THING       = find_alternator("IS_THING_EQUAL_TO_THING");
    this->alt_IS_THING_GREATER_THAN_THING   = find_alternator("IS_THING_GREATER_THAN_THING");
    this->alt_IS_THING_GREATER_OR_EQUAL_TO_THING = find_alternator("IS_THING_GREATER_OR_EQUAL_TO_THING");
    this->cmd_REGISTER_STREAMED_SCRIPT_INTERNAL = find_command("REGISTER_STREAMED_SCRIPT_INTERNAL");
}

void Commands::add_default_models(const std::map<std::string, uint32_t, iless>& default_models)
{
    for(auto& model_pair : default_models)
    {
        this->enum_carpedmodels->values.emplace(model_pair);
    }
}

optional<int32_t> Commands::find_constant(const string_view& value, bool context_free_only) const
{
    // TODO mayyybe speed up this? we didn't profile or anything.
    for(auto& enum_pair : enums)
    {
        if(enum_pair.second->is_global == context_free_only)
        {
            if(auto opt = enum_pair.second->find(value))
                return opt;
        }
    }
    return nullopt;
}

optional<int32_t> Commands::find_constant_all(const string_view& value) const
{
    // See https://github.com/thelink2012/gta3sc/issues/60
    if(auto opt = enum_carpedmodels->find(value))
        return opt;

    // TODO mayyybe speed up this? we didn't profile or anything.
    for(auto& enum_pair : enums)
    {
        if(&enum_pair.second == &enum_carpedmodels)
            continue;
        if(auto opt = enum_pair.second->find(value))
            return opt;
    }
    return nullopt;
}


optional<int32_t> Commands::find_constant_for_arg(const string_view& value, const Command::Arg& arg) const
{
    if(arg.type == ArgType::Constant)
    {
        if(auto opt_const = this->find_constant_all(value))
            return opt_const;
    }
    else
    {
        if(auto opt_const = arg.find_constant(value)) // constants stricly related to this Arg
            return opt_const;
    }

    // If the enum that the argument accepts is MODEL, and the above didn't find a match,
    // also try on the CARPEDMODEL enum.
    if(arg.uses_enum(this->enum_models))
    {
        if(auto opt_const = enum_carpedmodels->find(value))
            return opt_const;
    }

    if(arg.type != ArgType::Constant)
    {
        if(auto opt_const = this->find_constant(value, true)) // global constants
            return opt_const;
    }

    return nullopt;
}

//////////////

using MatchFailure = Commands::MatchFailure;

struct TagVar
{
    string_view ident;
};

static auto maybe_var_identifier(const string_view& ident, const Command::Arg& arginfo) -> optional<std::pair<string_view, bool>>
{
    if(arginfo.type != ArgType::TextLabel && arginfo.type != ArgType::TextLabel16 && arginfo.type != ArgType::AnyTextLabel)
    {
        return std::make_pair(ident, false);
    }
    else // TextLabel
    {
        if(arginfo.allow_global_var || arginfo.allow_local_var)
        {
            if(!arginfo.allow_constant)
                return std::make_pair(ident, false);
            else if(ident.size() && ident.front() == '$')
                return std::make_pair(ident.substr(1), true);
        }
        return nullopt;
    }
}

template<typename T>
static auto args_from_tree(const SyntaxTree& cmdnode) -> T
{
    T output;
    output.reserve(cmdnode.child_count() - 1);

    for(auto it = std::next(cmdnode.begin()); it != cmdnode.end(); ++it)
        output.emplace_back(it->get());

    return output;
}

static auto hint_from(optional<const SyntaxTree&> cmdnode) -> shared_ptr<const SyntaxTree>
{
    return cmdnode? cmdnode->shared_from_this() : nullptr;
}

static auto hint_from(optional<const SyntaxTree&> cmdnode, const Commands::MatchArgumentList::value_type& arg) -> shared_ptr<const SyntaxTree>
{
    if(is<const SyntaxTree*>(arg))
        return get<const SyntaxTree*>(arg)->shared_from_this();
    return hint_from(cmdnode);
}

static auto match_arg(const Commands& commands, const shared_ptr<const SyntaxTree>& hint,
                      int32_t arg, const Command::Arg& arginfo,
                      const SymTable& symtable, const shared_ptr<Scope>& scope_ptr) -> expected<const Command::Arg*, MatchFailure>
{
    if(arginfo.type == ArgType::Integer || arginfo.type == ArgType::Param)
        return &arginfo;
    return make_unexpected(MatchFailure{ hint, MatchFailure::ExpectedInt });
}

static auto match_arg(const Commands& commands, const shared_ptr<const SyntaxTree>& hint,
                      float arg, const Command::Arg& arginfo,
                      const SymTable& symtable, const shared_ptr<Scope>& scope_ptr) -> expected<const Command::Arg*, MatchFailure>
{
    if(arginfo.type == ArgType::Float || arginfo.type == ArgType::Param)
        return &arginfo;
    return make_unexpected(MatchFailure{ hint, MatchFailure::ExpectedFloat });
}

static auto match_arg(const Commands& commands, const shared_ptr<const SyntaxTree>& hint,
                      const TagVar& arg, const Command::Arg& arginfo,
                      const SymTable& symtable, const shared_ptr<Scope>& scope_ptr) -> expected<const Command::Arg*, MatchFailure>
{
    auto var_matches = [](const shared_ptr<Var>& var, const Command::Arg& arginfo) -> bool
    {
        switch(var->type)
        {
            case VarType::Int:
                return (arginfo.type == ArgType::Integer || arginfo.type == ArgType::Constant || arginfo.type == ArgType::Param);
            case VarType::Float:
                return (arginfo.type == ArgType::Float || arginfo.type == ArgType::Param);
            case VarType::TextLabel:
                return (arginfo.type == ArgType::TextLabel || arginfo.type == ArgType::AnyTextLabel);
            case VarType::TextLabel16:
                return (arginfo.type == ArgType::TextLabel16 || arginfo.type == ArgType::AnyTextLabel);
            default:
                Unreachable();
        }
    };

    if(!Miss2Identifier::is_identifier(arg.ident))
        return make_unexpected(MatchFailure{ hint, MatchFailure::InvalidIdentifier });

    if(auto var_ident = maybe_var_identifier(arg.ident, arginfo))
    {
        auto opt_token = Miss2Identifier::match(var_ident->first);
        if(!opt_token)
        {
            switch(opt_token.error())
            {
                case Miss2Identifier::InvalidIdentifier:return make_unexpected(MatchFailure{ hint, MatchFailure::InvalidIdentifier });
                case Miss2Identifier::NestingOfArrays:  return make_unexpected(MatchFailure{ hint, MatchFailure::IdentifierIndexNesting });
                case Miss2Identifier::NegativeIndex:    return make_unexpected(MatchFailure{ hint, MatchFailure::IdentifierIndexNegative });
                case Miss2Identifier::OutOfRange:       return make_unexpected(MatchFailure{ hint, MatchFailure::IdentifierIndexOutOfRange });
                default:                                Unreachable();
            }
        }

        auto& token = *opt_token;
        if(auto opt_var = symtable.find_var(token.identifier, scope_ptr))
        {
            if(token.index != nullopt && !is<size_t>(*token.index))
            {
                auto& index = get<string_view>(*token.index);
                if(auto opt_varidx = symtable.find_var(index, scope_ptr))
                {
                    if((*opt_varidx)->type != VarType::Int)
                        return make_unexpected(MatchFailure{ hint, MatchFailure::VariableIndexNotInt });
                    if((*opt_varidx)->count)
                        return make_unexpected(MatchFailure{ hint, MatchFailure::VariableIndexIsArray });
                }
                else if(commands.find_constant_all(index) == nullopt) // TODO -pedantic error
                {
                    return make_unexpected(MatchFailure{ hint, MatchFailure::VariableIndexNotVar });
                }
            }
            else if(token.index == nullopt && (*opt_var)->count != nullopt)
            {
                return make_unexpected(MatchFailure{ hint, MatchFailure::ExpectedVarIndex });
            }

            auto& var = *opt_var;
            if(!(arginfo.allow_global_var && var->global) && !(arginfo.allow_local_var && !var->global))
            {
                return make_unexpected(MatchFailure{ hint, MatchFailure::VariableKindNotAllowed });
            }

            if(!var_matches(var, arginfo))
                return make_unexpected(MatchFailure{ hint, MatchFailure::VariableTypeMismatch });

            return &arginfo;
        }
        else if(var_ident->second) // var not found, but required to find one?
        {
            return make_unexpected(MatchFailure{ hint, MatchFailure::ExpectedVar });
        }
    }

    return make_unexpected(MatchFailure{ hint, MatchFailure::NoSuchVar });
}

static auto match_arg(const Commands& commands, const shared_ptr<const SyntaxTree>& hint,
                      string_view text, const Command::Arg& arginfo,
                      const SymTable& symtable, const shared_ptr<Scope>& scope_ptr) -> expected<const Command::Arg*, MatchFailure>
{
    switch(arginfo.type)
    {
        case ArgType::Label:
            if(Miss2Identifier::is_identifier(text))
            {
                if(symtable.find_label(text))
                    return &arginfo;
                else
                    return make_unexpected(MatchFailure { hint, MatchFailure::NoSuchLabel });
            }
            else
                return make_unexpected(MatchFailure{ hint, MatchFailure::InvalidIdentifier });

        case ArgType::Constant:
            if(Miss2Identifier::is_identifier(text))
            {
                if(commands.find_constant_all(text))
                    return &arginfo;
                else
                    return make_unexpected(MatchFailure{ hint, MatchFailure::NoSuchConstant });
            }
            else
                return make_unexpected(MatchFailure{ hint, MatchFailure::InvalidIdentifier });

        case ArgType::TextLabel:
        case ArgType::TextLabel16:
        case ArgType::AnyTextLabel:
        {
            auto exp_var = match_arg(commands, hint, TagVar { text }, arginfo, symtable, scope_ptr);
            if(exp_var)
                return exp_var;
            else if(exp_var.error().reason == MatchFailure::NoSuchVar && arginfo.allow_constant)
                return &arginfo;
            else if(exp_var.error().reason == MatchFailure::InvalidIdentifier && arginfo.type == ArgType::AnyTextLabel && arginfo.allow_constant)
                return &arginfo;
            else
                return exp_var; // error state
        }

        case ArgType::Integer:
        case ArgType::Float:
        case ArgType::Param:
        {
            // HACK HACK HACK
            if(hint && hint->maybe_annotation<const StreamedFileAnnotation&>())
                return &arginfo;

            if(arginfo.allow_constant && arginfo.type != ArgType::Float)
            {
                if(commands.find_constant_for_arg(text, arginfo))
                    return &arginfo;
            }

            auto exp_var = match_arg(commands, hint, TagVar { text }, arginfo, symtable, scope_ptr);
            if(exp_var || exp_var.error().reason != MatchFailure::NoSuchVar)
                return exp_var;
            else if(arginfo.uses_enum(commands.get_models_enum())) // allow unknown models
                return &arginfo;
            else
                return make_unexpected(MatchFailure{ hint, MatchFailure::NoSuchVar });
        }

        default:
            Unreachable();
    }
}

static auto match_arg(const Commands& commands, const shared_ptr<const SyntaxTree>& hint,
                      const SyntaxTree& arg, const Command::Arg& arginfo,
                      const SymTable& symtable, const shared_ptr<Scope>& scope_ptr) -> expected<const Command::Arg*, MatchFailure>
{
    // TODO FIXME this matcher gives wrong error messages
    switch(arg.type())
    {
        case NodeType::Integer:
            return match_arg(commands, hint, 0, arginfo, symtable, scope_ptr);
        case NodeType::Float:
            return match_arg(commands, hint, 0.0f, arginfo, symtable, scope_ptr);
        case NodeType::Text:
            return match_arg(commands, hint, arg.text(), arginfo, symtable, scope_ptr);
        case NodeType::String:
            return make_unexpected(MatchFailure{ hint, MatchFailure::StringLiteralNotAllowed });
        default:
            Unreachable();
    }
}

auto Commands::match(const SyntaxTree& cmdnode, const SymTable& symtable, const shared_ptr<Scope>& scope_ptr) const -> expected<const Command*, MatchFailure>
{
    auto command_name = cmdnode.child(0).text();

    if(auto opt_alternator = this->find_alternator(command_name))
    {
        return this->match(*opt_alternator, cmdnode, symtable, scope_ptr);
    }
    else if(auto opt_command = this->find_command(command_name))
    {
        return this->match(*opt_command, cmdnode, symtable, scope_ptr);
    }
    else
    {
        return make_unexpected(MatchFailure { cmdnode.shared_from_this(), MatchFailure::NoCommandMatch });
    }
}

auto Commands::match(const Alternator& alternator, const SyntaxTree& cmdnode,
                     const SymTable& symtable, const shared_ptr<Scope>& scope_ptr) const -> expected<const Command*, MatchFailure>
{
    return this->match(alternator, cmdnode, args_from_tree<MatchArgumentList>(cmdnode), symtable, scope_ptr);
}

auto Commands::match(const Command& command, const SyntaxTree& cmdnode,
                     const SymTable& symtable, const shared_ptr<Scope>& scope_ptr) const -> expected<const Command*, MatchFailure>
{
    return this->match(command, cmdnode, args_from_tree<MatchArgumentList>(cmdnode), symtable, scope_ptr);
}

auto Commands::match(const Alternator& alternator, optional<const SyntaxTree&> cmdnode, const MatchArgumentList& args,
                     const SymTable& symtable, const shared_ptr<Scope>& scope_ptr) const -> expected<const Command*, MatchFailure>
{
    for(auto& cmd : alternator)
    {
        if(auto opt_command = this->match(*cmd, cmdnode, args, symtable, scope_ptr))
            return opt_command;
    }
    return make_unexpected(MatchFailure { hint_from(cmdnode), MatchFailure::NoAlternativeMatch });
}

auto Commands::match(const Command& command, optional<const SyntaxTree&> cmdnode, const MatchArgumentList& args,
                     const SymTable& symtable, const shared_ptr<Scope>& scope_ptr) const -> expected<const Command*, MatchFailure>
{
    size_t i = 0;
    expected<const Command::Arg*, MatchFailure> exp_arg;
    for(auto it = args.begin(); it != args.end(); ++it, ++i)
    {
        if(auto arginfo = command.arg(i))
        {
            if(is<int32_t>(*it))
                exp_arg = match_arg(*this, hint_from(cmdnode, *it), 0, *arginfo, symtable, scope_ptr);
            else if(is<float>(*it))
                exp_arg = match_arg(*this, hint_from(cmdnode, *it), 0.0f, *arginfo, symtable, scope_ptr);
            else // is<const SyntaxTree*>
                exp_arg = match_arg(*this, hint_from(cmdnode, *it), *get<const SyntaxTree*>(*it), *arginfo, symtable, scope_ptr);

            if(!exp_arg)
            {
                return make_unexpected(std::move(exp_arg.error()));
            }
        }
        else
        {
            return make_unexpected(MatchFailure{ hint_from(cmdnode, *it), MatchFailure::TooManyArgs });
        }
    }

    if(i < command.minimum_args())
        return make_unexpected(MatchFailure{ hint_from(cmdnode), MatchFailure::TooFewArgs });

    return &command;
}

void Commands::annotate(SyntaxTree& cmdnode, const Command& command,
                        const SymTable& symtable, const shared_ptr<Scope>& scope_ptr,
                        Script& script, ProgramContext& program) const
{
    return this->annotate(args_from_tree<AnnotateArgumentList>(cmdnode), command, symtable, scope_ptr, script, program);
}

void Commands::annotate(const AnnotateArgumentList& args, const Command& command,
                        const SymTable& symtable, const shared_ptr<Scope>& scope_ptr,
                        Script& script, ProgramContext& program) const
{
    // Expects all args to match command.args!

    auto find_var = [&](const string_view& value) -> optional<VarAnnotation>
    {
        auto opt_token = Miss2Identifier::match(value);
        if(!opt_token)
            return nullopt;

        auto& token = *opt_token;
        if(auto opt_var = symtable.find_var(token.identifier, scope_ptr))
        {
            using index_type = decltype(ArrayAnnotation::index);
            if(token.index == nullopt)
            {
                return VarAnnotation{ *opt_var, nullopt };
            }
            else if(is<size_t>(*token.index))
            {
                auto& index = get<size_t>(*token.index);
                return VarAnnotation{ *opt_var, index_type(index) };
            }
            else
            {
                auto& index = get<string_view>(*token.index);
                if(auto opt_varidx = symtable.find_var(index, scope_ptr))
                {
                    return VarAnnotation{ *opt_var, index_type(*opt_varidx) };
                }
                else if(auto opt_idx = this->find_constant_all(index))
                {
                    return VarAnnotation{ *opt_var, index_type(*opt_idx) };
                }
            }
        }

        return nullopt;
    };

    // TODO maybe start using VarAnnotation directly instead of converting to shared_ptr<Var>
    // Think of implications of this, such as the `any` type stack size.
    auto annotate_var = [](SyntaxTree& node, const VarAnnotation& annotation)
    {
        Expects(annotation.base != nullptr);
        if(annotation.index)
        {
            if(node.is_annotated())
                Expects(node.maybe_annotation<const ArrayAnnotation&>());
            else
                node.set_annotation(ArrayAnnotation{ annotation.base, *annotation.index });
        }
        else
        {
            if(node.is_annotated())
                Expects(node.maybe_annotation<const shared_ptr<Var>&>());
            else
                node.set_annotation(annotation.base);
        }
    };

    size_t i = 0;
    for(auto it = args.begin(); it != args.end(); ++it, ++i)
    {
        if(is<nullopt_t>(*it))
            continue;

        auto& node = *get<SyntaxTree*>(*it);
        auto& arginfo = *command.arg(i);

        switch(node.type())
        {
            case NodeType::Integer:
            {
                if(node.is_annotated())
                    Expects(node.maybe_annotation<const int32_t&>());
                else
                    node.set_annotation(static_cast<int32_t>(std::stoi(node.text().to_string(), nullptr, 0)));
                break;
            }

            case NodeType::Float:
            {
                if(node.is_annotated())
                    Expects(node.maybe_annotation<const float&>());
                else
                    node.set_annotation(std::stof(node.text().to_string()));
                break;
            }

            case NodeType::String:
            {
                // TODO, currently Unreachable() due to special handling of it.
                break;
            }

            case NodeType::Text:
            {
                if(node.maybe_annotation<const StreamedFileAnnotation&>())
                {
                    // hack for streamed script filenames instead of int value, do nothing
                }
                else if(arginfo.type == ArgType::Label)
                {
                    if(node.is_annotated())
                        Expects(node.maybe_annotation<const shared_ptr<Label>&>());
                    else
                        node.set_annotation(symtable.find_label(node.text()).value());
                }
                else if(arginfo.type == ArgType::TextLabel
                     || arginfo.type == ArgType::TextLabel16
                     || arginfo.type == ArgType::AnyTextLabel)
                {
                    if(auto opt_match = maybe_var_identifier(node.text(), arginfo))
                    {
                        if(auto opt_var = find_var(opt_match->first))
                        {
                            annotate_var(node, *opt_var);
                            break;
                        }
                    }

                    if(arginfo.allow_constant)
                    {
                        if(node.is_annotated())
                            Expects(node.maybe_annotation<const TextLabelAnnotation&>());
                        else
                        {
                            size_t limit = arginfo.type == ArgType::TextLabel?    7 :
                                           arginfo.type == ArgType::TextLabel16?  15 :
                                           arginfo.type == ArgType::AnyTextLabel? 127 : Unreachable();

                            if(node.text().size() > limit)
                                program.error(node, "identifier is too long, maximum size is {}", limit);
                            else
                                node.set_annotation(TextLabelAnnotation { arginfo.type != ArgType::TextLabel, node.text().to_string() });
                        }
                        break;
                    }

                    Unreachable();
                }
                else if(arginfo.type == ArgType::Integer || arginfo.type == ArgType::Float
                     || arginfo.type == ArgType::Constant || arginfo.type == ArgType::Param)
                {
                    if(auto opt_const = this->find_constant_for_arg(node.text(), arginfo))
                    {
                        if(node.is_annotated())
                            Expects(node.maybe_annotation<const int32_t&>());
                        else
                            node.set_annotation(*opt_const);
                        break;
                    }

                    bool is_model_enum = arginfo.uses_enum(this->get_models_enum());
                    // TODO VC miss2 doesn't allow models and vars with same name?

                    if(!is_model_enum || !program.is_model_from_ide(node.text()))
                    {
                        if(auto opt_var = find_var(node.text()))
                        {
                            bool was_annotated = node.is_annotated();
                            annotate_var(node, *opt_var);
                            if(!was_annotated) // i.e. do this only once
                            {
                                script.process_entity_type(node, arginfo.entity_type, !arginfo.allow_constant, program);
                                // TODO arginfo.out instead of !arginfo.allow_constant ^
                            }
                            break;
                        }
                    }

                    if(is_model_enum)
                    {
                        if(node.is_annotated())
                            Expects(node.maybe_annotation<const ModelAnnotation&>());
                        else
                        {
                            auto index = script.add_or_find_model(node.text());
                            assert(index >= 0);
                            node.set_annotation(ModelAnnotation{ script.shared_from_this(), uint32_t(index) });
                        }
                        break;
                    }
                    
                    Unreachable();
                }
                else
                {
                    Unreachable();
                }
                break;
            }

            default:
                Unreachable();
        }
    }

    Ensures(i >= command.minimum_args());
}

void Commands::MatchFailure::emit(ProgramContext& program)
{
    auto message = this->to_string();
    if(this->context)
        program.error(*this->context, message.c_str());
    else
        program.error(nocontext, message.c_str());
}

std::string Commands::MatchFailure::to_string()
{
    switch(this->reason)
    {
        case NoCommandMatch:            return "unknown command";
        case NoAlternativeMatch:        return "could not match alternative";
        case TooManyArgs:               return "too many arguments";
        case TooFewArgs:                return "too few arguments";
        case BadArgument:               return "bad argument";
        case ExpectedInt:               return "expected integer";
        case ExpectedFloat:             return "expected float";
        case NoSuchLabel:               return "no label with this name";
        case NoSuchConstant:            return "no string constant with this value";
        case NoSuchVar:                 return "no variable with this name";
        case ExpectedVar:               return "expected variable";
        case InvalidIdentifier:         return ::to_string(Miss2Identifier::InvalidIdentifier);
        case IdentifierIndexNesting:    return ::to_string(Miss2Identifier::NestingOfArrays);
        case IdentifierIndexNegative:   return ::to_string(Miss2Identifier::NegativeIndex);
        case IdentifierIndexOutOfRange: return ::to_string(Miss2Identifier::OutOfRange);
        case VariableIndexNotInt:       return "variable in index is not of INT type";
        case VariableIndexNotVar:       return "identifier between brackets is not a variable";
        case VariableIndexIsArray:      return "variable in index is of array type";
        case VariableKindNotAllowed:    return "variable kind (global/local) not allowed for this argument";
        case VariableTypeMismatch:      return "variable type does not match argument type";
        case StringLiteralNotAllowed:   return "STRING literal not allowed here";
        case ExpectedVarIndex:          return "use of array variable without a index";
        default:                        Unreachable();
    }
}

/////////////

static int xml_stoi(const char* string)
{
    try
    {
        return std::stoi(string, nullptr, 0);
    }
    catch(const std::exception& e)
    {
        throw ConfigError("couldn't convert string to int: {}", e.what());
    }
}

static bool xml_to_bool(const char* string)
{
    if(!strcmp(string, "true"))
        return true;
    else if(!strcmp(string, "false"))
        return false;
    else
        throw ConfigError("boolean is not 'true' or 'false', it is '{}'", string);
}

static bool xml_to_bool(const rapidxml::xml_attribute<>* attrib, bool default_value)
{
    return attrib? xml_to_bool(attrib->value()) : default_value;
}

static ArgType xml_to_argtype(const char* string)
{
    if(!strcmp(string, "INT"))
        return ArgType::Integer;
    else if(!strcmp(string, "FLOAT"))
        return ArgType::Float;
    else if(!strcmp(string, "PARAM"))
        return ArgType::Param;
    else if(!strcmp(string, "LABEL"))
        return ArgType::Label;
    else if(!strcmp(string, "BUFFER32"))
        return ArgType::Buffer32;
    else if(!strcmp(string, "CONST"))
        return ArgType::Constant;
    else if(!strcmp(string, "TEXT_LABEL"))
        return ArgType::TextLabel;
    else if(!strcmp(string, "TEXT_LABEL16"))
        return ArgType::TextLabel16;
    else if(!strcmp(string, "ANY_TEXT_LABEL"))
        return ArgType::AnyTextLabel;
    else
        throw ConfigError("unexpected 'Type' attribute: {}", string);
}

static void parse_enum_node(transparent_map<std::string, shared_ptr<Enum>>& enums, const rapidxml::xml_node<>* enum_node)
{
    using namespace rapidxml;

    xml_attribute<>* enum_name_attrib   = enum_node->first_attribute("Name");
    xml_attribute<>* enum_global_attrib = enum_node->first_attribute("Global");

    if(!enum_name_attrib)
        throw ConfigError("missing 'Name' attribute on '<Enum>' node");

    bool is_global = xml_to_bool(enum_global_attrib, false);
    auto eit = enums.find(enum_name_attrib->value());
    if(eit == enums.end())
    {
        auto enum_ptr = std::make_shared<Enum>(Enum { {}, is_global });
        eit = enums.emplace(enum_name_attrib->value(), std::move(enum_ptr)).first;
    }
    else
    {
        assert(is_global == eit->second->is_global);
    }

    std::map<std::string, int32_t, iless>& constant_map = eit->second->values;
    int32_t current_value = 0;

    for(auto value_node = enum_node->first_node(); value_node; value_node = value_node->next_sibling())
    {
        assert(!strcmp(value_node->name(), "Constant"));

        xml_attribute<>* value_name_attrib = value_node->first_attribute("Name");
        xml_attribute<>* value_value_attrib = value_node->first_attribute("Value");

        if(!value_name_attrib)
            throw ConfigError("missing 'Name' attribute on '<Constant>' node");

        if(value_value_attrib)
            current_value = xml_stoi(value_value_attrib->value());

        constant_map.emplace(value_name_attrib->value(), current_value);

        ++current_value;
    }
}

static std::pair<std::string, Command>
  parse_command_node(
      const rapidxml::xml_node<>* cmd_node,
      std::map<std::string, EntityType>& entities,
      const transparent_map<std::string, shared_ptr<Enum>>& enums)
{
    using namespace rapidxml;

    xml_attribute<>* id_attrib   = cmd_node->first_attribute("ID");
    xml_attribute<>* name_attrib = cmd_node->first_attribute("Name");
    xml_attribute<>* support_attrib = cmd_node->first_attribute("Supported");
    xml_node<>*      args_node   = cmd_node->first_node("Args");

    if(!id_attrib || !name_attrib)
        throw ConfigError("missing 'ID' or 'Name' attribute on '<Command>' node");

    std::vector<Command::Arg> args;

    if(args_node)
    {
        size_t num_args = 0;

        // Avoid realocations, find out the number of args beforehand.
        for(auto x = args_node->first_node(); x; x = x->next_sibling())
            ++num_args;

        args.reserve(num_args);

        for(auto arg_node = args_node->first_node("Arg"); arg_node; arg_node = arg_node->next_sibling("Arg"))
        {
            auto type_attrib         = arg_node->first_attribute("Type");
            auto out_attrib          = arg_node->first_attribute("Out");
            auto ref_attrib          = arg_node->first_attribute("Ref");
            auto optional_attrib     = arg_node->first_attribute("Optional");
            auto allow_const_attrib  = arg_node->first_attribute("AllowConst");
            auto allow_gvar_attrib   = arg_node->first_attribute("AllowGlobalVar");
            auto allow_lvar_attrib   = arg_node->first_attribute("AllowLocalVar");
            auto entity_attrib       = arg_node->first_attribute("Entity");
            auto enum_attrib         = arg_node->first_attribute("Enum");

            if(!type_attrib)
                throw ConfigError("missing 'Type' attribute on '<Arg>' node");

            // TODO OUT
            // TODO REF
            // Cannot build with a initializer list, VS goes mad :(
            Command::Arg arg;
            arg.type = xml_to_argtype(type_attrib->value());
            arg.optional = xml_to_bool(optional_attrib, false);
            arg.allow_constant = xml_to_bool(allow_const_attrib, true);
            arg.allow_global_var= xml_to_bool(allow_gvar_attrib, true);
            arg.allow_local_var = xml_to_bool(allow_lvar_attrib, true);
            arg.entity_type = 0;

            if(enum_attrib)
            {
                auto eit = enums.find(enum_attrib->value());
                if(eit != enums.end())
                {
                    arg.enums.emplace_back(eit->second);
                    arg.enums.shrink_to_fit();
                }
            }

            if(entity_attrib)
            {
                auto it = entities.emplace(entity_attrib->value(), EntityType(1 + entities.size())).first;
                arg.entity_type = it->second;
            }

            args.emplace_back(std::move(arg));
        }
    }

    return {
        name_attrib->value(),
        Command {
            xml_to_bool(support_attrib, true),               // supported
            uint16_t(xml_stoi(id_attrib->value()) & 0x7FFF), // id
            std::move(args),                                 // args
        }
    };
}

static std::pair<std::string, std::vector<const Command*>>
  parse_alternator_node(
      const rapidxml::xml_node<>* alt_node,
      const std::multimap<std::string, Command, iless>& commands)
{
    using namespace rapidxml;

    std::vector<const Command*> alternatives;

    xml_attribute<>* name_attrib = alt_node->first_attribute("Name");

    if(!name_attrib)
        throw ConfigError("missing 'Name' attribute on '<Alternator>' node");

    for(auto node = alt_node->first_node(); node; node = node->next_sibling())
    {
        xml_attribute<>* attrib = node->first_attribute("Name");

        if(!attrib)
            throw ConfigError("missing 'Name' attribute on '<Alternative>' node");

        auto it = commands.find(attrib->value());
        if(it != commands.end())
            alternatives.emplace_back(std::addressof(it->second));
    }

    return { name_attrib->value(), std::move(alternatives) };
}

Commands Commands::from_xml(const std::string& config_name, const std::vector<fs::path>& xml_list)
{
    using namespace rapidxml;

    // Order by priority (which should be read first).
    constexpr int XML_SECTION_CONSTANTS   = 1;
    constexpr int XML_SECTION_COMMANDS    = 2;
    constexpr int XML_SECTION_ALTERNATORS = 3;

    struct XmlData
    {
        std::string buffer;
        std::unique_ptr<xml_document<>> doc;
    };

    std::vector<XmlData> xml_vector;
    std::vector<std::pair<int, xml_node<>*>> xml_sections;

    std::multimap<std::string, Command, iless>                  commands;
    std::map<std::string, std::vector<const Command*>, iless>   alternators;
    std::map<std::string, EntityType>                           entities;
    transparent_map<std::string, shared_ptr<Enum>>              enums;

    // fundamental enums
    enums.emplace("MODEL", std::make_shared<Enum>(Enum { {}, false, }));
    enums.emplace("CARPEDMODEL", std::make_shared<Enum>(Enum { {}, false, }));

    auto xml_parse = [](const fs::path& path) -> XmlData
    {
        fs::path full_xml_path(path);
        try
        {
            auto opt_buffer = read_file_utf8(full_xml_path);
            if(opt_buffer == nullopt)
                throw ConfigError("failed to read xml {}: {}", full_xml_path.generic_u8string(), "could not open file for reading");

            auto doc = std::make_unique<xml_document<>>(); // buffer should be alive as long as doc
            doc->parse<0>(&(*opt_buffer)[0]); // buffer will get modified here

            return XmlData{ std::move(*opt_buffer), std::move(doc) };
        }
        catch(const rapidxml::parse_error& e)
        {
            throw ConfigError("failed to parse xml {}: {}", full_xml_path.generic_u8string(), e.what());
        }
        catch(const bad_optional_access& e)
        {
            throw ConfigError("failed to read xml {}: {}", full_xml_path.generic_u8string(), e.what());
        }
    };

    for(auto& xml_path : xml_list)
    {
        fs::path path;
        {
            if(!xml_path.is_absolute())
            {
                auto begin = xml_path.begin();
                if(begin != xml_path.end() && (*begin == "." || *begin == ".."))
                    path = xml_path;
                else
                    path = config_path() / config_name / xml_path;
            }
            else
            {
                path = xml_path;
            }
        }

        xml_vector.emplace_back(xml_parse(path));

        if(xml_node<>* root_node = xml_vector.back().doc->first_node("GTA3Script"))
        {
            for(auto node = root_node->first_node(); node; node = node->next_sibling())
            {
                if(!strcmp(node->name(), "Commands"))
                {
                    xml_sections.emplace_back(XML_SECTION_COMMANDS, node);
                }
                else if(!strcmp(node->name(), "Constants"))
                {
                    xml_sections.emplace_back(XML_SECTION_CONSTANTS, node);
                }
                else if(!strcmp(node->name(), "Alternators"))
                {
                    xml_sections.emplace_back(XML_SECTION_ALTERNATORS, node);
                }
            }
        }
    }

    std::stable_sort(xml_sections.begin(), xml_sections.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    for(auto& section_pair : xml_sections)
    {
        xml_node<>* node = section_pair.second;
        if(section_pair.first == XML_SECTION_COMMANDS)
        {
            for(auto cmd_node = node->first_node(); cmd_node; cmd_node = cmd_node->next_sibling())
            {
                if(!strcmp(cmd_node->name(), "Command"))
                {
                    auto cmd_pair = parse_command_node(cmd_node, entities, enums);
                    commands.emplace(std::move(cmd_pair));
                }
            }
        }
        else if(section_pair.first == XML_SECTION_CONSTANTS)
        {
            for(auto const_node = node->first_node(); const_node; const_node = const_node->next_sibling())
            {
                if(!strcmp(const_node->name(), "Enum"))
                {
                    parse_enum_node(enums, const_node);
                }
            }
        }
        else if(section_pair.first == XML_SECTION_ALTERNATORS)
        {
            for(auto alt_node = node->first_node(); alt_node; alt_node = alt_node->next_sibling())
            {
                if(!strcmp(alt_node->name(), "Alternator"))
                {
                    auto alt_pair = parse_alternator_node(alt_node, commands);
                    alternators.emplace(std::move(alt_pair));
                }
            }
        }
    }

    return Commands(std::move(commands), std::move(alternators), std::move(entities), std::move(enums));
}

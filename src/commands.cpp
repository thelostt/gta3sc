#include "commands.hpp"
#include "symtable.hpp"
#include "error.hpp"
#include "system.hpp"
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

// TODO every BadAlternator (which happens a lot because of argument matching)
//      we throw a exception (which is costful) and we also build up a ProgramError object (also costful).
//      Please improve this out.

Commands::Commands(std::multimap<std::string, Command> commands_, std::map<std::string, shared_ptr<Enum>> enums_)
    : commands(std::move(commands_)), enums(std::move(enums_))
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
    this->cmd_LOAD_AND_LAUNCH_MISSION       = find_command("LOAD_AND_LAUNCH_MISSION");
    this->cmd_START_NEW_SCRIPT              = find_command("START_NEW_SCRIPT");
    this->cmd_TERMINATE_THIS_SCRIPT         = find_command("TERMINATE_THIS_SCRIPT");
    this->cmd_RETURN                        = find_command("RETURN");
    this->cmd_RET                           = find_command("RET");
    this->cmd_GOTO                          = find_command("GOTO");
    this->cmd_GOTO_IF_FALSE                 = find_command("GOTO_IF_FALSE");
    this->cmd_ANDOR                         = find_command("ANDOR");
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
}

void Commands::add_default_models(const ProgramContext& program)
{
    for(auto& model_pair : program.default_models)
    {
        this->enum_carpedmodels->values.emplace(model_pair);
    }
}

static void match_identifier_var(const shared_ptr<Var>& var, const Command::Arg& arg, const SymTable& symbols)
{
    bool is_good = false;

    if((var->global && arg.allow_global_var) || (!var->global && arg.allow_local_var))
    {
        switch(var->type)
        {
            case VarType::Int:
                is_good = (arg.type == ArgType::Integer || arg.type == ArgType::Constant || arg.type == ArgType::Any);
                break;
            case VarType::Float:
                is_good = (arg.type == ArgType::Float || arg.type == ArgType::Any);
                break;
            case VarType::TextLabel:
                is_good = (arg.type == ArgType::TextLabel || arg.type == ArgType::Any);
                break;
            case VarType::TextLabel16:
                // TODO ArgType::TextLabel16?? (SA)
                is_good = (arg.type == ArgType::TextLabel || arg.type == ArgType::Any);
                break;
            default:
                Unreachable();
        }

        if(is_good == false)
            throw BadAlternator(nocontext, "XXX type mismatch");
    }
    else
        throw BadAlternator(nocontext, "XXX kind of var not allowed");
}

static bool match_var(const SyntaxTree& node, const Commands& commands, const Command::Arg& arg, const SymTable& symbols, const shared_ptr<Scope>& scope_ptr)
{
    if(auto opt_var = symbols.find_var(node.text(), scope_ptr))
    {
        try
        {
            match_identifier_var(*opt_var, arg, symbols);
            return true; // var exists and matches arg type
        }
        catch(const BadAlternator& error)
        {
            // var exists but arg type doesn't match var type
            throw BadAlternator(node, error);
        }
    }
    return false; // var doesn't exist
}

static void match_integer_constant(const SyntaxTree& node, const SymTable& symbols)
{
    switch(node.type())
    {
        case NodeType::Integer:
            return;
        case NodeType::Identifier:
            // TODO constants
            throw BadAlternator(node, "XXX match_integer_constant");
            break;
        default:
            throw BadAlternator(node, "XXX match_integer_constant not constant");
    }
}

static void match_identifier(const SyntaxTree& node, const Commands& commands, const Command::Arg& arg, const SymTable& symbols, const shared_ptr<Scope>& scope_ptr)
{
    switch(arg.type)
    {
        case ArgType::Label:
            if(!symbols.find_label(node.text()))
                throw BadAlternator(node, "XXX not label identifier");
            break;

        case ArgType::TextLabel:
            // Nothing to do, identifiers can be text labels with no checks.
            // TODO check for vars (SA)
            break;

        case ArgType::Integer:
        case ArgType::Float:
        case ArgType::Any:
        {
            if(arg.allow_constant && arg.type != ArgType::Float)
            {
                if(commands.find_constant_for_arg(node.text(), arg))
                    break;
            }

            if(match_var(node, commands, arg, symbols, scope_ptr))
                break;

            if(arg.uses_enum(commands.get_models_enum()))
            {
                // allow unknown models
                break;
            }

            throw BadAlternator(node, "XXX");
        }
        case ArgType::Constant:
        {
            if(commands.find_constant_all(node.text()))
                break;

            throw BadAlternator(node, "XXX");
        }

        default:
            Unreachable();
    }
}

template<typename Iter> static
const Command& match_internal(const Commands& commands, const SymTable& symbols, const shared_ptr<Scope>& scope_ptr,
    Commands::alternator_pair alternator_range, Iter begin, Iter end) // Iter should meet SyntaxTree** requiriments
{
    int TMP_bad_alter_id = -1; // TODO REMOVE ME
    auto num_target_args = (size_t)std::distance(begin, end);

    for(auto it = alternator_range.first; it != alternator_range.second; ++it)
    {
        const Command& alternative = it->second;

        bool is_optional = false;
        size_t args_readen = 0;

        auto it_alter_arg = alternative.args.begin();
        auto it_target_arg = begin; // SyntaxTree

        for(; ;
        (it_alter_arg->optional? it_alter_arg : ++it_alter_arg),
            ++it_target_arg,
            ++args_readen)
        {
            assert(args_readen <= num_target_args);

            if(args_readen < num_target_args)
            {
                if(it_alter_arg == alternative.args.end())
                    break; // too many args on target
            }
            else // end of arguments
            {
                if(it_alter_arg == alternative.args.end() || it_alter_arg->optional)
                    return alternative;
                else
                    break; // too few args on target
            }

            bool bad_alternative = false;

            switch((*it_target_arg)->type())
            {
                case NodeType::Integer:
                    bad_alternative = !(argtype_matches(it_alter_arg->type, ArgType::Integer) && it_alter_arg->allow_constant);
                    break;
                case NodeType::Float:
                    bad_alternative = !(argtype_matches(it_alter_arg->type, ArgType::Float) && it_alter_arg->allow_constant);
                    break;
                case NodeType::Array:
                    try
                    {
                        static Command::Arg array_index_arg(ArgType::Integer);

                        auto& var_node = (*it_target_arg)->child(0);
                        if(!match_var(var_node, commands, *it_alter_arg, symbols, scope_ptr))
                            throw BadAlternator(var_node, "XXX var array doesn't exist");

                        auto& index_node = (*it_target_arg)->child(1);
                        switch(index_node.type())
                        {
                            case NodeType::Integer:
                                // Fine as is.
                                break;
                            case NodeType::Identifier:
                                if(match_var(index_node, commands, array_index_arg, symbols, scope_ptr))
                                    break;
                                else if(commands.find_constant_all(index_node.text()) != nullopt)
                                    break; // check for -pedantic will happen in annotate 
                                else
                                    throw BadAlternator(index_node, "XXX array index identifier is not a var");
                                break;
                            default:
                                Unreachable();
                        }
                    }
                    catch(const BadAlternator&)
                    {
                        bad_alternative = true;
                    }
                    break;
                case NodeType::Identifier:
                    try
                    {
                        match_identifier(**it_target_arg, commands, *it_alter_arg, symbols, scope_ptr);
                    }
                    catch(const BadAlternator&)
                    {
                        bad_alternative = true;
                    }
                    break;
                case NodeType::ShortString:
                    bad_alternative = !(argtype_matches(it_alter_arg->type, ArgType::TextLabel) && it_alter_arg->allow_constant);
                    break;
                case NodeType::LongString:
                    bad_alternative = !(argtype_matches(it_alter_arg->type, ArgType::TextLabel) || argtype_matches(it_alter_arg->type, ArgType::Buffer128));
                    bad_alternative = bad_alternative || !it_alter_arg->allow_constant;;
                    break;
                default:
                    Unreachable();
            }

            if(bad_alternative)
            {
                TMP_bad_alter_id = (int) args_readen;
                break; // try another alternative
            }
        }
    }

    TMP_bad_alter_id = TMP_bad_alter_id < 0? 0 : TMP_bad_alter_id;
    if(begin != end) // TODO improve context
        throw BadAlternator(**(begin + TMP_bad_alter_id), "XXX BAD ALTERNATOR, GIVE ME A ERROR MESSAGE");
    else
        throw BadAlternator(nocontext, "XXX BAD ALTERNATOR, GIVE ME A ERROR MESSAGE");
}

template<typename Iter> static
void annotate_internal(const Commands& commands, const SymTable& symbols, const shared_ptr<Scope>& scope_ptr, Script& script, ProgramContext& program,
    const Command& command, Iter begin, Iter end)
{
    size_t i = 0;

    // Expects all command_args (begin,end) matches command.args

    for(auto it = begin; it != end; ++it)
    {
        auto& arg = command.args[i];
        if(!arg.optional) ++i;

        SyntaxTree& arg_node = **it;

        switch((*it)->type())
        {
            case NodeType::Integer:
            {
                if(arg_node.is_annotated())
                    Expects(arg_node.maybe_annotation<const int32_t&>());
                else
                    arg_node.set_annotation(static_cast<int32_t>(std::stoi(arg_node.text(), nullptr, 0)));
                break;
            }
            case NodeType::Float:
            {
                if(arg_node.is_annotated())
                    Expects(arg_node.maybe_annotation<const float&>());
                else
                    arg_node.set_annotation(std::stof(arg_node.text()));
                break;
            }

            case NodeType::ShortString:
            case NodeType::LongString:
            {
                // TODO unescape and annotate SA
                break;
            }

            case NodeType::Array:
            {
                Expects(arg.type == ArgType::Integer || arg.type == ArgType::Float || arg.type == ArgType::Any);

                auto& var_node = arg_node.child(0);
                auto& idx_node = arg_node.child(1);

                if(var_node.is_annotated())
                {
                    Expects(var_node.maybe_annotation<const shared_ptr<Var>&>());
                    Expects(idx_node.maybe_annotation<const shared_ptr<Var>&>() || arg_node.child(1).maybe_annotation<int32_t>());
                }
                else
                {
                    auto opt_var_base = symbols.find_var(var_node.text(), scope_ptr);
                    Expects(opt_var_base != nullopt);
                    var_node.set_annotation(*opt_var_base);

                    switch(idx_node.type())
                    {
                        case NodeType::Integer:
                            if(idx_node.is_annotated())
                                Expects(idx_node.maybe_annotation<const int32_t&>());
                            else
                                idx_node.set_annotation(static_cast<int32_t>(std::stoi(idx_node.text(), nullptr, 0)));
                            break;
                        case NodeType::Identifier:
                            if(auto opt_var_idx = symbols.find_var(idx_node.text(), scope_ptr))
                            {
                                if(idx_node.is_annotated())
                                    Expects(idx_node.maybe_annotation<const shared_ptr<Var>&>());
                                else
                                    idx_node.set_annotation(*opt_var_idx);
                            }
                            else if(auto opt_idx = commands.find_constant_all(idx_node.text()))
                            {
                                if(idx_node.is_annotated())
                                    Expects(idx_node.maybe_annotation<const int32_t&>());
                                else
                                {
                                    if(program.opt.pedantic)
                                        program.error(idx_node, "XXX constant in array index is not -pedantic");

                                    idx_node.set_annotation(static_cast<int32_t>(*opt_idx));
                                }
                            }
                            else
                                Unreachable();
                            break;
                        default:
                            Unreachable();
                    }
                }
                break;
            }

            case NodeType::Identifier:
            {
                if(arg.type == ArgType::Label)
                {
                    if(arg_node.is_annotated())
                        Expects(arg_node.maybe_annotation<const shared_ptr<Label>&>());
                    else
                    {
                        shared_ptr<Label> label_ptr = symbols.find_label(arg_node.text()).value();
                        arg_node.set_annotation(label_ptr);
                    }
                }
                else if(arg.type == ArgType::TextLabel)
                {
                    if(arg_node.is_annotated())
                        Expects(arg_node.maybe_annotation<const std::string&>());
                    else
                        arg_node.set_annotation(arg_node.text());
                }
                else if(arg.type == ArgType::Integer || arg.type == ArgType::Float || arg.type == ArgType::Constant || arg.type == ArgType::Any)
                {
                    if(auto opt_const = commands.find_constant_for_arg(arg_node.text(), arg))
                    {
                        if(arg_node.is_annotated())
                            Expects(arg_node.maybe_annotation<const int32_t&>());
                        else
                            arg_node.set_annotation(*opt_const);
                    }
                    else
                    {
                        bool is_model_enum = arg.uses_enum(commands.get_models_enum());
                        bool avoid_var = is_model_enum && program.is_model_from_ide(arg_node.text());
                        // TODO VC miss2 doesn't allow models and vars with same name?

                        if(auto opt_var = !avoid_var? symbols.find_var(arg_node.text(), scope_ptr) : nullopt)
                        {
                             if(arg_node.is_annotated())
                                Expects(arg_node.maybe_annotation<const shared_ptr<Var>&>());
                            else
                                arg_node.set_annotation(*opt_var);
                        }
                        else if(is_model_enum)
                        {
                            if(arg_node.is_annotated())
                                Expects(arg_node.maybe_annotation<const ModelAnnotation&>());
                            else
                            {
                                auto index = script.add_or_find_model(arg_node.text());
                                assert(index >= 0); // at first we don't use a negative based indice, that is changed later before the compilation step.
                                arg_node.set_annotation(ModelAnnotation{ script.shared_from_this(), uint32_t(index) });
                            }
                        }
                        else
                            Unreachable();
                    }
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
}

////////

const Command& Commands::match(const SyntaxTree& command_node, const SymTable& symbols, const shared_ptr<Scope>& scope_ptr) const
{
    auto alternator_range = commands.equal_range(command_node.child(0).text());
    return ::match_internal(*this, symbols, scope_ptr, alternator_range, command_node.begin() + 1, command_node.end());
}

const Command& Commands::match_internal(const SymTable& symbols, const shared_ptr<Scope>& scope_ptr,
    alternator_pair alternator_range, const SyntaxTree** begin, const SyntaxTree** end) const
{
    return ::match_internal(*this, symbols, scope_ptr, alternator_range, begin, end);
}

void Commands::annotate(SyntaxTree& command_node, const Command& command,
    const SymTable& symbols, const shared_ptr<Scope>& scope_ptr, Script& script, ProgramContext& program) const
{
    return ::annotate_internal(*this, symbols, scope_ptr, script, program, command, command_node.begin() + 1, command_node.end());
}

void Commands::annotate_internal(const SymTable& symbols, const shared_ptr<Scope>& scope_ptr, Script& script, ProgramContext& program,
    const Command& command, SyntaxTree** begin, SyntaxTree** end) const
{
    return ::annotate_internal(*this, symbols, scope_ptr, script, program, command, begin, end);
}

optional<int32_t> Commands::find_constant(const std::string& value, bool context_free_only) const
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

optional<int32_t> Commands::find_constant_all(const std::string& value) const
{
    // TODO mayyybe speed up this? we didn't profile or anything.
    for(auto& enum_pair : enums)
    {
        if(auto opt = enum_pair.second->find(value))
            return opt;
    }
    return nullopt;
}


optional<int32_t> Commands::find_constant_for_arg(const std::string& value, const Command::Arg& arg) const
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

/////////////

static int xml_stoi(const char* string)
{
    try
    {
        return std::stoi(string, nullptr, 0);
    }
    catch(const std::exception& e)
    {
        throw ConfigError("Couldn't convert string to int: {}", e.what());
    }
}

static bool xml_to_bool(const char* string)
{
    if(!strcmp(string, "true"))
        return true;
    else if(!strcmp(string, "false"))
        return false;
    else
        throw ConfigError("Boolean is not 'true' or 'false', it is '{}'", string);
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
    else if(!strcmp(string, "TEXT_LABEL"))
        return ArgType::TextLabel;
    else if(!strcmp(string, "ANY"))
        return ArgType::Any;
    else if(!strcmp(string, "LABEL"))
        return ArgType::Label;
    else if(!strcmp(string, "BUFFER"))
        return ArgType::Buffer128;
    else if(!strcmp(string, "CONST"))
        return ArgType::Constant;
    else
        throw ConfigError("Unexpected Type attribute: {}", string);
}

static void parse_enum_node(std::map<std::string, shared_ptr<Enum>>& enums, const rapidxml::xml_node<>* enum_node)
{
    using namespace rapidxml;

    xml_attribute<>* enum_name_attrib   = enum_node->first_attribute("Name");
    xml_attribute<>* enum_global_attrib = enum_node->first_attribute("Global");

    if(!enum_name_attrib)
        throw ConfigError("Missing Name attrib on <Enum> node");

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
            throw ConfigError("Missing Name attrib on <Constant> node");

        if(value_value_attrib)
            current_value = xml_stoi(value_value_attrib->value());

        constant_map.emplace(value_name_attrib->value(), current_value);

        ++current_value;
    }
}

static std::pair<std::string, Command>
  parse_command_node(const rapidxml::xml_node<>* cmd_node, const std::map<std::string, shared_ptr<Enum>>& enums)
{
    using namespace rapidxml;

    xml_attribute<>* id_attrib   = cmd_node->first_attribute("ID");
    xml_attribute<>* name_attrib = cmd_node->first_attribute("Name");
    xml_attribute<>* support_attrib = cmd_node->first_attribute("Supported");
    xml_node<>*      args_node   = cmd_node->first_node("Args");

    if(!id_attrib || !name_attrib)
        throw ConfigError("Missing ID or Name attrib on <Command> node");

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
                throw ConfigError("Missing Type attrib on <Arg> node");

            // TODO OUT
            // TODO REF
            // TODO ENTITY
            // Cannot build with a initializer list, VS goes mad :(
            Command::Arg arg;
            arg.type = xml_to_argtype(type_attrib->value());
            arg.optional = xml_to_bool(optional_attrib, false);
            arg.allow_constant = xml_to_bool(allow_const_attrib, true);
            arg.allow_global_var= xml_to_bool(allow_gvar_attrib, true);
            arg.allow_local_var = xml_to_bool(allow_lvar_attrib, true);

            if(enum_attrib)
            {
                auto eit = enums.find(enum_attrib->value());
                if(eit != enums.end())
                {
                    arg.enums.emplace_back(eit->second);
                    arg.enums.shrink_to_fit();
                }
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

Commands Commands::from_xml(const std::vector<fs::path>& xml_list)
{
    using namespace rapidxml;

    std::multimap<std::string, Command>     commands;
    std::map<std::string, shared_ptr<Enum>> enums;

    // fundamental enums
    enums.emplace("MODEL", std::make_shared<Enum>(Enum { {}, false, }));
    enums.emplace("CARPEDMODEL", std::make_shared<Enum>(Enum { {}, false, }));

    fs::path conf_path = config_path();

    for(auto& xml_path : xml_list)
    {
        std::string xml_data;
        xml_document<> doc;   // xml_data should be alive as long as doc

        fs::path full_xml_path(conf_path / xml_path);
        try
        {
            xml_data = read_file_utf8(full_xml_path).value();

            if(xml_data.empty())
                continue;

            doc.parse<0>(&xml_data[0]); // xml_data will get modified here
        }
        catch(const rapidxml::parse_error& e)
        {
            throw ConfigError("Failed to parse XML {}: {}", full_xml_path.generic_u8string(), e.what());
        }
        catch(const bad_optional_access& e)
        {
            throw ConfigError("Failed to read XML {}: {}", full_xml_path.generic_u8string(), e.what());
        }

        if(xml_node<>* root_node = doc.first_node("GTA3Script"))
        {
            for(auto node = root_node->first_node(); node; node = node->next_sibling())
            {
                if(!strcmp(node->name(), "Commands"))
                {
                    for(auto cmd_node = node->first_node(); cmd_node; cmd_node = cmd_node->next_sibling())
                    {
                        if(!strcmp(cmd_node->name(), "Command"))
                        {
                            auto cmd_pair = parse_command_node(cmd_node, enums);
                            commands.emplace(std::move(cmd_pair));
                        }
                    }
                }
                else if(!strcmp(node->name(), "Constants"))
                {
                    for(auto const_node = node->first_node(); const_node; const_node = const_node->next_sibling())
                    {
                        if(!strcmp(const_node->name(), "Enum"))
                        {
                            parse_enum_node(enums, const_node);
                        }
                    }
                }
            }
        }

    }

    return Commands(std::move(commands), std::move(enums));
}

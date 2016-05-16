#include "defs.hpp"

Commands gta3_commands()
{
    auto mgr       = gtavc_commands();
    auto& commands = mgr.commands;
    auto& enums    = mgr.enums;

    auto cmd_arg = [&](const char* name, size_t i) -> Command::Arg& {
        return commands.find(name)->second.args[i];
    };

    ////////////////

    auto gta3_models_enum = make_enum({
        { "CAR_CHEETAH", 105 },
    });

    mgr.enums["GTA3_DEFAULT_MODELS"] = gta3_models_enum;

    ////////////////

    cmd_arg("REQUEST_MODEL", 0).enums.emplace_back(gta3_models_enum);
    cmd_arg("HAS_MODEL_LOADED", 0).enums.emplace_back(gta3_models_enum);
    cmd_arg("MARK_MODEL_AS_NO_LONGER_NEEDED", 0).enums.emplace_back(gta3_models_enum);
    cmd_arg("CREATE_CAR", 0).enums.emplace_back(gta3_models_enum);

    /////////////// CLEO2
    commands.emplace(decltype(mgr.commands)::value_type
    {
        "CALL",
        {
            true,
            0x0AB1,
            {
                { ArgType::Label, false, true, false, false, {} },
                { ArgType::Integer, false, true, false, false, {} },
                { ArgType::Any, true, true, true, true,{} },
            }
        }
    });

    commands.emplace(decltype(mgr.commands)::value_type
    {
        "RET",
        {
            true,
            0x0AB2,
            {
                { ArgType::Integer, false, true, false, false,{} },
                { ArgType::Any, true, true, true, true,{} },
            }
        }
    });

    commands.emplace(decltype(mgr.commands)::value_type
    {
        "CALL_FUNCTION",
        {
            true,
            0x0AA5,
            {
                { ArgType::Integer, false, true, true, true, {} },
                { ArgType::Integer, false, true, true, true, {} },
                { ArgType::Integer, false, true, true, true, {} },
                { ArgType::Any, true, true, true, true, {} },
            }
        }
    });

    commands.emplace(decltype(mgr.commands)::value_type
    {
        "CALL_METHOD",
        {
            true,
            0x0AA6,
            {
                { ArgType::Integer, false, true, true, true,{} },
                { ArgType::Integer, false, true, true, true,{} },
                { ArgType::Integer, false, true, true, true,{} },
                { ArgType::Integer, false, true, true, true,{} },
                { ArgType::Any, true, true, true, true,{} },
            }
        }
    });

    commands.emplace(decltype(mgr.commands)::value_type
    {
        "CALL_FUNCTION_RETURN",
        {
            true,
            0x0AA7,
            {
                { ArgType::Integer, false, true, true, true,{} },
                { ArgType::Integer, false, true, true, true,{} },
                { ArgType::Integer, false, true, true, true,{} },
                { ArgType::Any, true, true, true, true,{} },
            }
        }
    });

    commands.emplace(decltype(mgr.commands)::value_type
    {
        "CALL_METHOD_RETURN",
        {
            true,
            0x0AA8,
            {
                { ArgType::Integer, false, true, true, true,{} },
                { ArgType::Integer, false, true, true, true,{} },
                { ArgType::Integer, false, true, true, true,{} },
                { ArgType::Integer, false, true, true, true,{} },
                { ArgType::Any, true, true, true, true,{} },
            }
        }
    });

    commands.emplace(decltype(mgr.commands)::value_type
    {
        "POP_FLOAT",
        {
            true,
            0x0AE9,
            {
                { ArgType::Float, false, false, true, true,{} },
            }
        }
    });

    commands.emplace(decltype(mgr.commands)::value_type
    {
        "WRITE_MEMORY",
        {
            true,
            0x0A8C,
            {
                { ArgType::Integer, false, true, true, true, {} },
                { ArgType::Integer, false, true, true, true, {} },
                { ArgType::Integer, false, true, true, true, {} },
                { ArgType::Integer, false, true, true, true, {} },
            }
        }
    });

    mgr.update();
    return mgr;
}

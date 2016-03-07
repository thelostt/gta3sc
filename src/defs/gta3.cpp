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

    return mgr;
}

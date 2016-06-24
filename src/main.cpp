#include "stdinc.h"
#include "parser.hpp"
#include "error.hpp"
#include "symtable.hpp"
#include "commands.hpp"
#include "compiler.hpp"
#include "disassembler.hpp"
#include "decompiler.hpp"
#include "codegen.hpp"
#include "program.hpp"
#include "system.hpp"
#include "cpp/argv.hpp"

int compile(fs::path input, fs::path output, const Options& options, const Commands& commands);
int decompile(fs::path input, fs::path output, const Options& options, const Commands& commands);

const char* help_message =
R"(Usage: gta3sc [compile|decompile] file --config=<name> [options]
Options:
  --help                   Display this information
  -o <file>                Place the output into <file>
  --config=<name>          Which compilation configurations to use (gta3,gtavc,
                           gtasa). This effectively reads the data files at
                           '/config/<name>/' and sets some appropriate flags.
  -pedantic                Forbid the usage of extensions not in R* compiler.
  -f[no-]half-float        Whether codegen uses GTA III half-float format.
  -f[no-]text-label-prefix Whether codegen uses GTA SA text label data type.
)";

enum class Action
{
    None,
    Compile,
    Decompile,
};

int main(int argc, char** argv)
{
    // use fprintf(stderr, ...) for error reporting since we don't have a ProgramContext on main.

    Action action = Action::None;
    Options options;
    fs::path input, output;
    std::string config_name;
    optional<Commands> commands;        // can't construct Commands with no arguments

    ++argv;

    if(*argv && **argv != '-')
    {
        if(!strcmp(*argv, "compile"))
        {
            ++argv;
            action = Action::Compile;
        }

        if(!strcmp(*argv, "decompile"))
        {
            ++argv;
            action = Action::Decompile;
        }
    }

    try
    {
        bool flag;

        while(*argv)
        {
            if(**argv != '-')
            {
                if(!input.empty())
                    throw invalid_opt("input file appears twice");

                input = *argv;
                ++argv;
            }
            else if(optget(argv, "-h", "--help", 0))
            {
                fprintf(stdout, "%s", help_message);
                return EXIT_SUCCESS;
            }
            else if(const char* o = optget(argv, "-o", nullptr, 1))
            {
                output = o;
            }
            else if(optget(argv, nullptr, "-pedantic", 0))
            {
                options.pedantic = true;
            }
            else if(const char* name = optget(argv, nullptr, "--config", 1))
            {
                config_name = name;

                // TODO instead of hard-coding the flags, use a XML?
                if(config_name == "gta3")
                {
                    options.use_half_float = true;
                    options.has_text_label_prefix = false;
                }
                else if(config_name == "gtavc")
                {
                    options.use_half_float = false;
                    options.has_text_label_prefix = false;
                }
                else if(config_name == "gtasa")
                {
                    options.use_half_float = false;
                    options.has_text_label_prefix = true;
                }
                else
                {
                    throw invalid_opt("arbitrary config names not supported yet. Must be 'gta3', 'gtavc' or 'gtasa'");
                }
            }
            else if(optflag(argv, "half-float", &flag))
            {
                options.use_half_float = flag;
            }
            else if(optflag(argv, "text-label-prefix", &flag))
            {
                options.has_text_label_prefix = flag;
            }
            else
            {
                throw invalid_opt(fmt::format("unregonized argument '{}'", *argv));
            }
        }
    }
    catch(const invalid_opt& e)
    {
        fprintf(stderr, "gta3sc: error: %s\n", e.what());
        return EXIT_FAILURE;
    }

    if(input.empty())
    {
        fprintf(stderr, "gta3sc: error: no input file\n");
        return EXIT_FAILURE;
    }

    if(config_name.empty())
    {
        fprintf(stderr, "gta3sc: error: no game config specified [--config=<name>]\n");
        return EXIT_FAILURE;
    }

    if(action == Action::None)
    {
        std::string extension = input.extension().string();
        if(iequal_to()(extension, ".sc"))
            action = Action::Compile;
        else if(iequal_to()(extension, ".scm"))
            action = Action::Decompile;
        else if(iequal_to()(extension, ".scc"))
            action = Action::Decompile;
        else if(iequal_to()(extension, ".cs"))
            action = Action::Decompile;
        else if(iequal_to()(extension, ".cm"))
            action = Action::Decompile;
        else
        {
            fprintf(stderr, "gta3sc: error: could not infer action from input extension (compile/decompile)\n");
            return EXIT_FAILURE;
        }
    }

    fs::path conf_path = config_path();
    fprintf(stdout, "Using '%s' as configuration path\n", conf_path.string().c_str());

    try
    {
        commands = Commands::from_xml({ config_name + "/constants.xml", config_name + "/commands.xml" });
    }
    catch(const ConfigError& e)
    {
        fprintf(stderr, "gta3sc: error: %s\n", e.what());
    }

    switch(action)
    {
        case Action::Compile:
            return compile(input, output, options, *commands);
        case Action::Decompile:
            return decompile(input, output, options, *commands);
        default:
            Unreachable();
    }
}

int compile(fs::path input, fs::path output, const Options& options, const Commands& commands)
{
    if(output.empty())
    {
        // TODO .cs .scc
        output = input;
        output.replace_extension(".scm");
    }

    try {
        std::vector<shared_ptr<Script>> scripts;

        ProgramContext program(options);

        //const char* input = "intro.sc";
        //const char* input = "test.sc";
        //const char* input = "gta3_src/main.sc";

        auto main = Script::create(input, ScriptType::Main);
        auto symbols = SymTable::from_script(*main, commands, program);
        symbols.apply_offset_to_vars(2);

        scripts.emplace_back(main);

        auto subdir = main->scan_subdir();

        auto ext_scripts = read_and_scan_symbols(subdir, symbols.extfiles.begin(), symbols.extfiles.end(), ScriptType::MainExtension, commands, program);
        auto sub_scripts = read_and_scan_symbols(subdir, symbols.subscript.begin(), symbols.subscript.end(), ScriptType::Subscript, commands, program);
        auto mission_scripts = read_and_scan_symbols(subdir, symbols.mission.begin(), symbols.mission.end(), ScriptType::Mission, commands, program);

        // TODO handle lex/parser errors

        for(auto& x : ext_scripts)
        {
            symbols.merge(std::move(x.second), program);
            scripts.emplace_back(x.first); // maybe move
        }

        for(auto& x : sub_scripts)
        {
            symbols.merge(std::move(x.second), program);
            scripts.emplace_back(x.first); // maybe move
        }

        for(size_t i = 0; i < mission_scripts.size(); ++i)
        {
            auto& x = mission_scripts[i];
            symbols.merge(std::move(x.second), program);
            scripts.emplace_back(x.first); // maybe move
            scripts.back()->mission_id = i;
        }

        symbols.check_command_count(program);

        symbols.build_script_table(scripts);

        for(auto& script : scripts)
        {
            script->annotate_tree(symbols, commands, program);
        }

        // not thread-safe
        std::vector<std::string> models = Script::compute_unknown_models(scripts);

        // CompilerContext wants an annotated ASTs, if we have any error, it's possible that
        // the AST is not correctly annotated.
        if(program.has_error())
            throw HaltJobException();

        std::vector<CodeGenerator> gens;
        gens.reserve(scripts.size());
        for(auto& script : scripts)
        {
            CompilerContext cc(script, symbols, commands, program);
            cc.compile();
            gens.emplace_back(std::move(cc), program);
        }

        // Codegen expects a successful compilation.
        if(program.has_error())
            throw HaltJobException();

        size_t global_vars_size = 0;
        if(auto highest_var = symbols.highest_global_var())
        {
            global_vars_size = (*highest_var)->end_offset();
        }

        CompiledScmHeader header(CompiledScmHeader::Version::Liberty, symbols.size_global_vars(), models, scripts);

        // not thread-safe
        Expects(gens.size() == scripts.size());
        for(size_t i = 0; i < gens.size(); ++i) // zip
        {
            scripts[i]->size = gens[i].compute_labels();                    // <- maybe???! this is actually thread safe
        }                                                                   //
        Script::compute_script_offsets(scripts, header.compiled_size());    // <- but this isn't


        for(auto& gen : gens)
            gen.generate();

        if(FILE* f = u8fopen(output, "wb"))
        {
            auto guard = make_scope_guard([&] {
                fclose(f);
            });

            if(true)
            {
                CodeGeneratorData hgen(header, program);
                hgen.generate();
                write_file(f, hgen.buffer(), hgen.buffer_size());
            }

            for(auto& gen : gens)
            {
                write_file(f, gen.buffer(), gen.buffer_size());
            }
        }
        else
        {
            program.error(nocontext, "XXX");
        }

        if(program.has_error())
            throw HaltJobException();

    } catch(const HaltJobException&) {
        // TODO put a error message of compilation failed instead of zeroing output!??!!!
        FILE* f = u8fopen(output, "wb");
        if(f) fclose(f);
    }

    return 0;
}

int decompile(fs::path input, fs::path output, const Options& options, const Commands& commands)
{
    ProgramContext program(options);

    FILE* outstream;

    if(output.empty())
    {
        outstream = stdout;
    }
    else
    {
        outstream = u8fopen(output, "wb");
        if(!outstream)
            program.fatal_error(nocontext, "Could not open file {} for writing", output.generic_u8string());
    }

    auto guard = make_scope_guard([&] {
        if(outstream != stdout) fclose(outstream);
    });

    auto opt_decomp = Disassembler::from_file(options, commands, input);
    if(!opt_decomp)
        program.fatal_error(nocontext, "File {} does not exist", input.generic_u8string());

    Disassembler decomp(std::move(*opt_decomp));
    auto scm_header = decomp.read_header(DecompiledScmHeader::Version::Liberty).value();
    decomp.analyze_header(scm_header);
    decomp.run_analyzer();
    auto data = decomp.get_data();

    fprintf(outstream, "%s\n", DecompilerContext(commands, std::move(data)).decompile().c_str());

    return 0;
}

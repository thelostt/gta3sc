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

int compile(fs::path input, fs::path output, ProgramContext& program);
int decompile(fs::path input, fs::path output, ProgramContext& program);

const char* help_message =
R"(Usage: gta3sc [compile|decompile] file --config=<name> [options]
Options:
  --help                   Display this information
  -o <file>                Place the output into <file>
  --headerless             Does not generate a header on the output SCM.
  --header=<version>       Generates the specified header version (gta3,gtavc,
                           gtasa). Ignored if --headerless specified.
  --config=<name>          Which compilation configurations to use (gta3,gtavc,
                           gtasa). This effectively reads the data files at
                           '/config/<name>/' and sets some appropriate flags.
  --datadir=<path>         Path to where IDE and DAT files are in.
                           The compiler will still try to behave properly
                           without this, but this is still recommended.
  -pedantic                Forbid the usage of extensions not in R* compiler.
  --guesser                Allows the use of language features not completly
                           known or understood by the modding community.
  -f[no-]half-float        Codegen uses GTA III half-float format.
  -f[no-]text-label-prefix Codegen uses GTA SA text label data type.
  -f[no-]skip-if           Omits compiling ANDOR for single condition statements.
  -f[no-]optimize-zero     Compiles 0.0 as 0, using a 8 bit data type.
  -f[no-]entity-tracking   Tracks entity types in variables.
  -f[no-]script-name-check Checks if there's duplicated SCRIPT_NAMEs.
  -f[no-]switch            Enables the SWITCH statement.
  -fbreak-continue         Allows the use of BREAK and CONTINUE in all
                           statements, including WHILE and REPEAT.
  -f[no-]scope-then-label  When combined with -pedantic, outputs a error message
                           whenever a label is used before a curly bracket
                           instead of after.
  -f[no-]arrays            Enables the use of arrays.
  -f[no-]streamed-scripts  Enables the use of streamed scripts and generates an
                           associated script.img archive.
)";

enum class Action
{
    None,
    Compile,
    Decompile,
};

int main(int argc, char** argv)
{
    // Due to main() not having a ProgramContext yet, error reporting must be done using fprintf(stderr, ...).

    Action action = Action::None;
    Options options;
    fs::path input, output;
    std::string config_name;
    optional<ProgramContext> program; // delay construction of ProgramContext
    std::map<std::string, uint32_t, iless> default_models;
    std::map<std::string, uint32_t, iless> level_models;

    fs::path datadir;
    const char* levelfile = nullptr;

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
                {
                    fprintf(stderr, "gta3sc: error: input file appears twice.\n");
                    return EXIT_FAILURE;
                }

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
            else if(optget(argv, nullptr, "--guesser", 0))
            {
                options.guesser = true;
            }
            else if(optget(argv, nullptr, "--headerless", 0))
            {
                options.headerless = true;
            }
            else if(const char* name = optget(argv, nullptr, "--config", 1))
            {
                config_name = name;

                // TODO instead of hard-coding the flags, use a XML?
                if(config_name == "gta3")
                {
                    levelfile = "gta3.dat";
                    options.header = Options::HeaderVersion::GTA3;
                    options.use_half_float = true;
                    options.has_text_label_prefix = false;
                    options.skip_single_ifs = false;
                    options.fswitch = false;
                    options.scope_then_label = false;
                    options.farrays = false;
                    options.streamed_scripts = false;
                }
                else if(config_name == "gtavc")
                {
                    levelfile = "gta_vc.dat";
                    options.header = Options::HeaderVersion::GTAVC;
                    options.use_half_float = false;
                    options.has_text_label_prefix = false;
                    options.skip_single_ifs = false;
                    options.fswitch = false;
                    options.scope_then_label = true;
                    options.farrays = false;
                    options.streamed_scripts = false;
                }
                else if(config_name == "gtasa")
                {
                    levelfile = "gta.dat";
                    options.header = Options::HeaderVersion::GTASA;
                    options.use_half_float = false;
                    options.has_text_label_prefix = true;
                    options.skip_single_ifs = false;
                    options.fswitch = true;
                    options.scope_then_label = true;
                    options.farrays = true;
                    options.streamed_scripts = true;
                }
                else
                {
                    fprintf(stderr, "gta3sc: error: arbitrary config names not supported yet, must be 'gta3', 'gtavc' or 'gtasa'.\n");
                    return EXIT_FAILURE;
                }
            }
            else if(const char* path = optget(argv, nullptr, "--datadir", 1))
            {
                datadir = path;
            }
            else if(optflag(argv, "half-float", &flag))
            {
                options.use_half_float = flag;
            }
            else if(optflag(argv, "text-label-prefix", &flag))
            {
                options.has_text_label_prefix = flag;
            }
            else if(optflag(argv, "skip-if", &flag))
            {
                options.skip_single_ifs = flag;
            }
            else if(optflag(argv, "optimize-zero", &flag))
            {
                options.optimize_zero_floats = flag;
            }
            else if(optflag(argv, "entity-tracking", &flag))
            {
                options.entity_tracking = flag;
            }
            else if(optflag(argv, "script-name-check", &flag))
            {
                options.script_name_check = flag;
            }
            else if(optflag(argv, "switch", &flag))
            {
                options.fswitch = flag;
            }
            else if(optflag(argv, "break-continue", nullptr))
            {
                options.allow_break_continue = true;
            }
            else if(optflag(argv, "scope-then-label", &flag))
            {
                options.scope_then_label = flag;
            }
            else if(optflag(argv, "arrays", &flag))
            {
                options.farrays = flag;
            }
            else if(optflag(argv, "streamed-scripts", &flag))
            {
                options.streamed_scripts = flag;
            }
            else
            {
                fprintf(stderr, "gta3sc: error: unregonized argument '%s'\n", *argv);
                return EXIT_FAILURE;
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

    if(!options.guesser && options.fswitch)
    {
        fprintf(stderr, "gta3sc: error: use of -fswitch only available in guesser mode [--guesser]\n");
        return EXIT_FAILURE;
    }

    if(!datadir.empty())
    {
        if(levelfile == nullptr)
        {
            if(fs::exists(datadir / "gta.dat"))
                levelfile = "gta.dat";
            else if(fs::exists(datadir / "gta3.dat"))
                levelfile = "gta3.dat";
            else if(fs::exists(datadir / "gta_vc.dat"))
                levelfile = "gta_vc.dat";
            else
            {
                fprintf(stderr, "gta3sc: error: could not find level file (gta*.dat) in datadir '%s'.\n",
                            datadir.generic_u8string().c_str());
                return EXIT_FAILURE;
            }
        }

        try
        {
            default_models = load_dat(datadir / "default.dat", true);
            level_models   = load_dat(datadir / levelfile, false);
        }
        catch(const ConfigError& e)
        {
            fprintf(stderr, "gta3sc: error: %s\n", e.what());
            return EXIT_FAILURE;
        }
    }

    try
    {
        std::vector<fs::path> config_files;
        config_files.reserve(3);

        config_files.emplace_back(config_name + "/commands.xml");
        config_files.emplace_back(config_name + "/constants.xml");
        if(datadir.empty()) config_files.emplace_back(config_name + "/default.xml");

        Commands commands = Commands::from_xml(config_files);
        commands.add_default_models(default_models);

        program.emplace(std::move(options), std::move(commands));
        program->setup_models(std::move(default_models), std::move(level_models));
    }
    catch(const ConfigError& e)
    {
        fprintf(stderr, "gta3sc: error: %s\n", e.what());
        return EXIT_FAILURE;
    }

    fs::path conf_path = config_path();
    fprintf(stdout, "gta3sc: using '%s' as configuration path.\n", conf_path.generic_u8string().c_str());

    switch(action)
    {
        case Action::Compile:
            return compile(input, output, *program);
        case Action::Decompile:
            return decompile(input, output, *program);
        default:
            Unreachable();
    }
}

int compile(fs::path input, fs::path output, ProgramContext& program)
{
    if(output.empty())
    {
        // TODO .cs .scc
        output = input;
        output.replace_extension(".scm");
    }

    try {
        std::vector<shared_ptr<Script>> scripts;

        //const char* input = "intro.sc";
        //const char* input = "test.sc";
        //const char* input = "gta3_src/main.sc";

        auto main = Script::create(program, input, ScriptType::Main);
        auto symbols = SymTable::from_script(*main, program);
        symbols.apply_offset_to_vars(2);

        scripts.emplace_back(main);

        auto subdir = main->scan_subdir();

        auto ext_scripts = read_and_scan_symbols(subdir, symbols.extfiles.begin(), symbols.extfiles.end(), ScriptType::MainExtension, program);
        auto sub_scripts = read_and_scan_symbols(subdir, symbols.subscript.begin(), symbols.subscript.end(), ScriptType::Subscript, program);
        auto mission_scripts = read_and_scan_symbols(subdir, symbols.mission.begin(), symbols.mission.end(), ScriptType::Mission, program);
        auto streamed_scripts = read_and_scan_symbols(subdir, symbols.streamed.begin(), symbols.streamed.end(), ScriptType::StreamedScript, program);

        // Following steps wants a fully working syntax tree, so check for parser/lexer errors.
        if(program.has_error())
            throw HaltJobException();

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

        {
            size_t i = 0;
            for(auto& x : mission_scripts)
            {
                symbols.merge(std::move(x.second), program);
                scripts.emplace_back(x.first); // maybe move
                scripts.back()->mission_id = static_cast<uint16_t>(i++);
            }
        }

        {
            size_t i = 0;
            for(auto& x : streamed_scripts)
            {
                symbols.merge(std::move(x.second), program);
                scripts.emplace_back(x.first); // maybe move
                scripts.back()->streamed_id = static_cast<uint16_t>(i++);
            }
        }

        symbols.check_command_count(program);

        symbols.build_script_table(scripts);

        for(auto& script : scripts)
        {
            script->annotate_tree(symbols, program);
        }

        // not thread-safe
        std::vector<std::string> models = Script::compute_unknown_models(scripts);

        // not thread-safe
        Script::verify_entity_types(scripts, symbols, program);

        // not thread-safe
        Script::verify_script_names(scripts, program);

        // CompilerContext wants an annotated ASTs, if we have any error, it's possible that
        // the AST is not correctly annotated.
        if(program.has_error())
            throw HaltJobException();

        std::vector<CodeGenerator> gens;
        gens.reserve(scripts.size());
        for(auto& script : scripts)
        {
            CompilerContext cc(script, symbols, program);
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

        CompiledScmHeader header(program.opt.get_header<CompiledScmHeader::Version>(),
                                 symbols.size_global_vars(), models, scripts);

        // not thread-safe
        Expects(gens.size() == scripts.size());
        for(size_t i = 0; i < gens.size(); ++i) // zip
        {
            scripts[i]->size = gens[i].compute_labels();                    // <- maybe???! this is actually thread safe
        }                                                                   //
        Script::compute_script_offsets(scripts, header.compiled_size());    // <- but this isn't


        for(auto& gen : gens)
            gen.generate();

        auto generate_output = [&]
        {
            FILE *f = 0, *script_img = 0;
            std::vector<std::reference_wrapper<const CodeGenerator>> into_script_img;

            auto guard = make_scope_guard([&] {
                if(f) fclose(f);
                if(script_img) fclose(script_img);
            });

            f = u8fopen(output, "wb");
            if(f == nullptr)
            {
                program.fatal_error(nocontext, "XXX  failed to open output for writing");
                //return;
            }

            if(program.opt.streamed_scripts)
            {
                script_img = u8fopen(fs::path(output).replace_filename("script.img"), "wb");
                if(!script_img)
                {
                    program.fatal_error(nocontext, "XXX failed to open script.img for writing");
                    //return;
                }
            }

            if(!program.opt.headerless)
            {
                CodeGeneratorData hgen(header, program);
                hgen.generate();
                write_file(f, hgen.buffer(), hgen.buffer_size());
            }

            for(auto& gen : gens)
            {
                if(gen.script->type != ScriptType::StreamedScript)
                    write_file(f, gen.buffer(), gen.buffer_size());
                else
                    into_script_img.emplace_back(std::cref(gen));
            }

            if(program.opt.streamed_scripts)
            {
                // TODO endian independent img building

                struct alignas(4) CdHeader
                {
                    char magic[4];
                    uint32_t num_entries;
                };

                struct alignas(4) CdEntry
                {
                    uint32_t offset;                // in sectors
                    uint16_t streaming_size;        // in sectors
                    uint16_t size_in_archive = 0;   // in sectors
                    char     filename[24];
                };

                struct alignas(4) AAAScript
                {
                    uint32_t size_global_space;
                    uint8_t num_streams_allocated;
                    uint8_t unknown1  = 2;
                    uint16_t unknown2 = 0;
                };

                auto round_2kb = [](size_t size) -> size_t
                {
                    return (size + 2048 - 1) & ~(2048 - 1);
                };

                AAAScript aaa_scm;
                aaa_scm.size_global_space = header.size_global_vars_space - 8;
                aaa_scm.num_streams_allocated = 62; // TODO

                CdHeader cd_header { {'V','E','R','2'}, static_cast<uint32_t>(1 + into_script_img.size()) };
                std::vector<CdEntry> directory;
                directory.reserve(1 + into_script_img.size());

                std::string temp_filename;
                size_t files_offset = round_2kb(sizeof(CdHeader) + ((1 + into_script_img.size()) * sizeof(CdEntry)));
                size_t end_offset = 0;

                auto add_entry = [&](const char* filename, size_t size)
                {
                    CdEntry next_entry;

                    if(directory.empty())
                        next_entry.offset = round_2kb(files_offset) / 2048;
                    else
                        next_entry.offset = directory.back().offset + directory.back().streaming_size;

                    next_entry.streaming_size = static_cast<uint16_t>(round_2kb(size) / 2048);

                    strncpy(next_entry.filename, filename, 23);
                    next_entry.filename[23] = 0;

                    directory.emplace_back(next_entry);
                };

                add_entry("aaa.scm", 8);
                for(auto& into : into_script_img)
                {
                    const Script& script = *into.get().script;
                    temp_filename = script.path.stem().u8string();
                    temp_filename += ".scm";
                    add_entry(temp_filename.c_str(), script.size.value());
                }

                // TODO, fseek past eof isn't well-defined by the standard, do it in another way
                // TODO check return status of fseek
                
                if(directory.empty())
                    end_offset = files_offset;
                else
                    end_offset = (directory.back().offset + directory.back().streaming_size) * 2048;

                fseek(script_img, end_offset - 1, SEEK_SET);
                fputc(0, script_img);
                fseek(script_img, 0, SEEK_SET);

                write_file(script_img, &cd_header, sizeof(cd_header));
                write_file(script_img, directory.data(), sizeof(CdEntry) * directory.size());

                fseek(script_img, directory[0].offset * 2048, SEEK_SET); // aaa.scm
                write_file(script_img, &aaa_scm, sizeof(aaa_scm));

                for(size_t i = 0; i < into_script_img.size(); ++i)
                {
                    const CodeGenerator& gen = into_script_img[i].get();
                    fseek(script_img, directory[1+i].offset * 2048, SEEK_SET);
                    write_file(script_img, gen.buffer(), gen.buffer_size());
                }
            }
        };

        generate_output();

        if(program.has_error())
            throw HaltJobException();

    } catch(const HaltJobException&) {
        // TODO put a error message of compilation failed instead of zeroing output!??!!!
        FILE* f = u8fopen(output, "wb");
        if(f) fclose(f);
        return EXIT_FAILURE;
    }

    return 0;
}

int decompile(fs::path input, fs::path output, ProgramContext& program)
{
    const Commands& commands = program.commands;

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

    auto opt_decomp = Disassembler::from_file(program, commands, input);
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

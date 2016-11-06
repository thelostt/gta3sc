#include "stdinc.h"
#include "parser.hpp"
#include "error.hpp"
#include "symtable.hpp"
#include "commands.hpp"
#include "compiler.hpp"
#include "disassembler.hpp"
#include "decompiler.hpp"
#include "decompiler_ir2.hpp"
#include "codegen.hpp"
#include "program.hpp"
#include "system.hpp"
#include "cpp/argv.hpp"
#include "cdimage.hpp"

int compile(fs::path input, fs::path output, ProgramContext&);
int decompile(fs::path input, fs::path output, ProgramContext&);

template<typename OnOutput>
bool decompile(const void* bytecode, size_t bytecode_size,
               const void* script_img, size_t script_img_size,
               ProgramContext&, Options::Lang, OnOutput);


const char* help_message =
R"(Usage: gta3sc [compile|decompile] file --config=<name> [options]
Options:
  --help                   Display this information
  -o <file>                Place the output into <file>
  --config=<name>          Which compilation configurations to use (gta3,gtavc,
                           gtasa). This effectively reads the data files at
                           '/config/<name>/' and sets some appropriate flags.
  --datadir=<path>         Path to where IDE and DAT files are in.
                           The compiler will still try to behave properly
                           without this, but this is still recommended.
  -pedantic                Forbid the usage of extensions not in R* compiler.
  --guesser                Allows the use of language features not completly
                           known or understood by the modding community.
  -D <name>                Defines the preprocessor directive <name>.
  -U <name>                Undefines the preprocessor directive <name>.
  -emit-ir2                Emits a explicit IR based on Sanny Builder syntax.
  -fsyntax-only            Only checks the syntax, i.e. doesn't generate code.
  -fentity-tracking        Tracks entity types in variables.
  -fscript-name-check      Checks for duplicate SCRIPT_NAMEs.
  -fbreak-continue         Allows the use of BREAK and CONTINUE in all
                           statements, including WHILE and REPEAT.
  -fstreamed-scripts       Enables the use of streamed scripts and generates an
                           associated script.img archive.
  -fscope-then-label       When combined with -pedantic, outputs a error message
                           whenever a label is used before a curly bracket
                           instead of after.
  -fswitch                 Enables the SWITCH statement.
  -farrays                 Enables the use of arrays.
  -ftext-label-vars        Enables VAR_TEXT_LABEL and VAR_TEXT_LABEL16.
  -mno-header              Does not generate a header on the output SCM.
  -mheader=<version>       Generates the specified header version (gta3,gtavc,
                           gtasa).
  -mlocal-offsets          Label offsets are referenced locally and relative
                           to the offset 0 of the compiled script.
  -mq11.4                  Codegen uses GTA III half-float format.
  -mtyped-text-label       Codegen uses GTA SA text label data type.
  -mskip-if                Omits compiling ANDOR on single condition statements.
  -moptimize-zero          Compiles 0.0 as 0, using a 8 bit data type.
  -flocal-var-limit=<n>    The index limit of local variables.
  -fmission-var-limit=<n>  The index limit of mission local variables. Defaults
                           to -flocal-var-limit if not set.
  -ftimer-index=<n>        The local variable index of TIMERA.
  -fswitch-case-limit=<n>  The limit on the number of CASE in a SWITCH.
  -fskip-cutscene          Enables the use of SKIP_CUTSCENE_START.
  --linear-sweep           Disassembler scans the code by the means of a
                           linear-sweep instead of a recursive traversal.
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
        uint32_t temp_i32;

        while(*argv)
        {
            if(**argv != '-')
            {
                if(!input.empty())
                {
                    fprintf(stderr, "gta3sc: error: input file appears twice\n");
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
            else if(optget(argv, nullptr, "--linear-sweep", 0))
            {
                options.linear_sweep = true;
            }
            else if(const char* name = optget(argv, nullptr, "--config", 1))
            {
                config_name = name;

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
                    options.text_label_vars = false;
                    options.skip_cutscene = false;
                    options.timer_index = 16;
                    options.local_var_limit = 16;
                    options.mission_var_limit = nullopt;
                    options.switch_case_limit = nullopt;
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
                    options.text_label_vars = false;
                    options.skip_cutscene = false;
                    options.timer_index = 16;
                    options.local_var_limit = 16;
                    options.mission_var_limit = nullopt;
                    options.switch_case_limit = nullopt;
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
                    options.text_label_vars = true;
                    options.skip_cutscene = true;
                    options.timer_index = 32;
                    options.local_var_limit = 32;
                    options.mission_var_limit = 1024;
                    options.switch_case_limit = 75;
                }
                else
                {
                    fprintf(stderr, "gta3sc: error: arbitrary config names not supported yet, must be 'gta3', 'gtavc' or 'gtasa'\n");
                    return EXIT_FAILURE;
                }
            }
            else if(const char* path = optget(argv, nullptr, "--datadir", 1))
            {
                datadir = path;
            }
            else if(const char* ver = optget(argv, nullptr, "-mheader", 1))
            {
                if(!strcmp(ver, "gta3"))
                    options.header = Options::HeaderVersion::GTA3;
                else if(!strcmp(ver, "gtavc"))
                    options.header = Options::HeaderVersion::GTAVC;
                else if(!strcmp(ver, "gtasa"))
                    options.header = Options::HeaderVersion::GTASA;
                else
                {
                    fprintf(stderr, "gta3sc: error: invalid header version, must be 'gta3', 'gtavc' or 'gtasa'\n");
                    return EXIT_FAILURE;
                }
            }
            else if(optflag(argv, "-mno-header", nullptr))
            {
                options.headerless = true;
            }
            else if(optflag(argv, "-mq11.4", &flag))
            {
                options.use_half_float = flag;
            }
            else if(optflag(argv, "-mtyped-text-label", &flag))
            {
                options.has_text_label_prefix = flag;
            }
            else if(optflag(argv, "-mskip-if", &flag))
            {
                options.skip_single_ifs = flag;
            }
            else if(optflag(argv, "-moptimize-zero", &flag))
            {
                options.optimize_zero_floats = flag;
            }
            else if(optflag(argv, "-fentity-tracking", &flag))
            {
                options.entity_tracking = flag;
            }
            else if(optflag(argv, "-fscript-name-check", &flag))
            {
                options.script_name_check = flag;
            }
            else if(optflag(argv, "-fswitch", &flag))
            {
                options.fswitch = flag;
            }
            else if(optflag(argv, "-fbreak-continue", nullptr))
            {
                options.allow_break_continue = true;
            }
            else if(optflag(argv, "-fscope-then-label", &flag))
            {
                options.scope_then_label = flag;
            }
            else if(optflag(argv, "-farrays", &flag))
            {
                options.farrays = flag;
            }
            else if(optflag(argv, "-fstreamed-scripts", &flag))
            {
                options.streamed_scripts = flag;
            }
            else if(optflag(argv, "-fskip-cutscene", &flag))
            {
                options.skip_cutscene = flag;
            }
            else if(optflag(argv, "-mlocal-offsets", nullptr))
            {
                options.use_local_offsets = true;
            }
            else if(optint(argv, "-ftimer-index", &options.timer_index)) {}
            else if(optint(argv, "-flocal-var-limit", &options.local_var_limit)) {}
            else if(optint(argv, "-fmission-var-limit", &temp_i32))
            {
                options.mission_var_limit = temp_i32;
            }
            else if(optint(argv, "-fswitch-case-limit", &temp_i32))
            {
                options.switch_case_limit = temp_i32;
            }
            else if(optflag(argv, "-fsyntax-only", nullptr))
            {
                options.fsyntax_only = true;
            }
            else if(optflag(argv, "-emit-ir2", nullptr))
            {
                options.emit_ir2 = true;
            }
            else if(const char* name = optget(argv, "-D", nullptr, 1))
            {
                options.define(name);
            }
            else if(const char* name = optget(argv, "-U", nullptr, 1))
            {
                options.undefine(name);
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

    if(!options.guesser && options.streamed_scripts)
    {
        fprintf(stderr, "gta3sc: error: use of -fstreamed_scripts only available in guesser mode [--guesser]\n");
        return EXIT_FAILURE;
    }

    if(!options.guesser && options.skip_cutscene)
    {
        fprintf(stderr, "gta3sc: error: use of -fskip-cutscene only available in guesser mode [--guesser]\n");
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
                fprintf(stderr, "gta3sc: error: could not find level file (gta*.dat) in datadir '%s'\n",
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
        config_files.reserve(4);

        config_files.emplace_back(config_name + "/alternators.xml");
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
    //fprintf(stderr, "gta3sc: using '%s' as configuration path\n", conf_path.generic_u8string().c_str());

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
    // TODO cleanup this function

    if(output.empty())
    {
        // if fsyntax-only bla bla
        // TODO .cs .scc
        output = input;
        output.replace_extension(program.opt.emit_ir2? ".ir2" : ".scm");
    }

    try
    {
        std::vector<shared_ptr<Script>> scripts;

        //const char* input = "intro.sc";
        //const char* input = "test.sc";
        //const char* input = "gta3_src/main.sc";

        auto main = Script::create(program, input, ScriptType::Main);

        if(main == nullptr)
        {
            Expects(program.has_error());
            throw HaltJobException();
        }

        auto symbols = SymTable::from_script(*main, program);
        symbols.apply_offset_to_vars(2);

        scripts.emplace_back(main);

        auto subdir = main->scan_subdir();

        std::vector<std::pair<shared_ptr<Script>, SymTable>> ext_scripts;
        {
            std::set<std::string, iless> extfiles_readen;
            std::deque<std::string>     extfiles_stack;

            std::copy(symbols.extfiles.begin(), symbols.extfiles.end(), std::back_inserter(extfiles_stack));

            while(!extfiles_stack.empty())
            {
                auto top = std::move(extfiles_stack.front());
                extfiles_stack.pop_front();

                if(!extfiles_readen.count(top))
                {
                    if(auto script_pair = read_and_scan_symbols(subdir, top, ScriptType::MainExtension, program))
                    {
                        ext_scripts.emplace_back(std::move(*script_pair));
                        auto& script_syms = ext_scripts.back().second;

                        extfiles_readen.emplace(std::move(top));
                        std::copy(script_syms.extfiles.begin(), script_syms.extfiles.end(), std::back_inserter(extfiles_stack));
                    }
                }
            }
        }

        // merge symbols from extension scripts here, since we need symbols.subscripts/missions/streamed
        // with the content from both main and main extensions
        for(auto& x : ext_scripts)
        {
            symbols.merge(std::move(x.second), program);
            scripts.emplace_back(x.first); // maybe move
        }

        auto sub_scripts = read_and_scan_symbols(subdir, symbols.subscript.begin(), symbols.subscript.end(), ScriptType::Subscript, program);
        auto mission_scripts = read_and_scan_symbols(subdir, symbols.mission.begin(), symbols.mission.end(), ScriptType::Mission, program);
        auto streamed_scripts = read_and_scan_symbols(subdir, symbols.streamed.begin(), symbols.streamed.end(), ScriptType::StreamedScript, program);

        // Following steps wants a fully working syntax tree, so check for parser/lexer errors.
        if(program.has_error())
            throw HaltJobException();

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

        // Do not perform code gen if checking only syntax
        if(program.opt.fsyntax_only)
            return 0;

        size_t global_vars_size = 0;
        if(auto highest_var = symbols.highest_global_var())
        {
            global_vars_size = (*highest_var)->end_offset();
        }

        CompiledScmHeader header(program.opt.get_header<CompiledScmHeader::Version>(),
                                 symbols.size_global_vars(), models, scripts);

        size_t header_size = program.opt.headerless? 0 : header.compiled_size();

        // not thread-safe
        Expects(gens.size() == scripts.size());
        for(size_t i = 0; i < gens.size(); ++i) // zip
        {
            scripts[i]->size = gens[i].compute_labels();                    // <- maybe???! this is actually thread safe
        }                                                                   //
        Script::compute_script_offsets(scripts, header_size);               // <- but this isn't
            


        for(auto& gen : gens)
            gen.generate();

        auto generate_output = [&](auto& main_scm, auto& script_img, bool has_script_img)
        {
            std::vector<std::reference_wrapper<const CodeGenerator>> into_script_img;

            // TODO allocate_file_space(main_scm, ...)

            if(!program.opt.headerless)
            {
                CodeGeneratorData hgen(header, program);
                hgen.generate();
                write_file(main_scm, hgen.buffer(), hgen.buffer_size());
            }

            for(auto& gen : gens)
            {
                if(gen.script->type != ScriptType::StreamedScript)
                    write_file(main_scm, gen.buffer(), gen.buffer_size());
                else
                    into_script_img.emplace_back(std::cref(gen));
            }

            std::sort(into_script_img.begin(), into_script_img.end(), [](const auto& lhs, const auto& rhs) {
                // TODO this creates a string every sorting pass! Fix this.
                auto& lhs_script = *lhs.get().script;
                auto& rhs_script = *rhs.get().script;
                return iless()(lhs_script.path.stem().u8string(), rhs_script.path.stem().u8string());
            });

            if(has_script_img)
            {
                // TODO endian independent img building

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

                // TODO check return status of allocate_file_space
                
                if(directory.empty())
                    end_offset = files_offset;
                else
                    end_offset = (directory.back().offset + directory.back().streaming_size) * 2048;

                allocate_file_space(script_img, end_offset);

                write_file(script_img, 0, &cd_header, sizeof(cd_header));
                write_file(script_img, sizeof(cd_header), directory.data(), sizeof(CdEntry) * directory.size());

                // aaa.scm
                write_file(script_img, directory[0].offset * 2048, &aaa_scm, sizeof(aaa_scm));

                for(size_t i = 0; i < into_script_img.size(); ++i)
                {
                    const CodeGenerator& gen = into_script_img[i].get();
                    write_file(script_img, directory[1+i].offset * 2048, gen.buffer(), gen.buffer_size());
                }
            }
        };

        bool use_script_img = program.opt.streamed_scripts && !program.opt.headerless;

        if(program.opt.emit_ir2)
        {
            FILE *outstream = 0;
            std::vector<uint8_t> main_scm;
            std::vector<uint8_t> script_img;

            auto guard = make_scope_guard([&] {
                if(outstream && outstream != stdout) fclose(outstream);
            });

            // TODO remove this reserve once we use allocate_file for main_scm in generate_output
            main_scm.reserve(2048 * 10);

            outstream = (output != "-"? u8fopen(output, "wb") : stdout);
            if(outstream == nullptr)
                program.fatal_error(nocontext, "failed to open output for writing");

            bool is_first_line = true;
            auto print_ir2_line = [&](const std::string& line)
            {
                if(is_first_line)
                {
                    is_first_line = false;
                    fprintf(outstream, "%s", line.c_str());
                }
                else
                {
                    fprintf(outstream, "\n%s", line.c_str());
                }
            };

            generate_output(main_scm, script_img, use_script_img);

            auto status = decompile(main_scm.data(), main_scm.size(),
                                    script_img.data(), script_img.size(), program,
                                    Options::Lang::IR2, print_ir2_line);
            if(!status)
                throw HaltJobException();

            //fputc('<', outstream);
        }
        else
        {
            FILE *main_scm = 0, *script_img = 0;

            auto guard = make_scope_guard([&] {
                if(main_scm) fclose(main_scm);
                if(script_img) fclose(script_img);
            });

            main_scm = u8fopen(output, "wb");
            if(main_scm == nullptr)
                program.fatal_error(nocontext, "failed to open output for writing");

            if(use_script_img)
            {
                script_img = u8fopen(fs::path(output).replace_filename("script.img"), "wb");
                if(!script_img)
                    program.fatal_error(nocontext, "failed to open script.img for writing");
            }

            generate_output(main_scm, script_img, use_script_img);
        }
        

        if(program.has_error())
            throw HaltJobException();

        return 0;
    }
    catch(const HaltJobException&)
    {
        fprintf(stderr, "gta3sc: compilation failed\n");
        return EXIT_FAILURE;
    }
}



int decompile(fs::path input, fs::path output, ProgramContext& program)
{
    if(output.empty())
    {
        output = input;
        output.replace_extension(program.opt.emit_ir2? ".ir2" : ".sc");
    }

    try
    {
        const Commands& commands = program.commands;

        FILE* outstream;

        auto lang = (program.opt.emit_ir2? Options::Lang::IR2 : Options::Lang::GTA3Script);

        auto guard = make_scope_guard([&] {
            if(outstream != stdout) fclose(outstream);
        });

        if(lang == Options::Lang::GTA3Script)
            program.fatal_error(nocontext, "GTA3script output is disabled, please use -emit-ir2 for IR2 output");

        outstream = (output != "-"? u8fopen(output, "wb") : stdout);
        if(!outstream)
            program.fatal_error(nocontext, "could not open file '{}' for writing", output.generic_u8string());

        auto opt_bytecode = read_file_binary(input);
        if(!opt_bytecode)
            program.fatal_error(nocontext, "file '{}' does not exist", input.generic_u8string());

        std::vector<uint8_t> script_img;
        if(program.opt.streamed_scripts)
        {
            auto img_path = fs::path(input).replace_filename("script.img");
            if(auto opt = read_file_binary(img_path))
                script_img = std::move(*opt);
            else
                program.fatal_error(nocontext, "file '{}' does not exist", img_path.generic_u8string());
        }

        auto println = [&](const std::string line) { fprintf(outstream, "%s\n", line.c_str()); }; 
        if(!decompile(opt_bytecode->data(), opt_bytecode->size(), script_img.data(), script_img.size(), program, lang, println))
            throw HaltJobException();

        return 0;
    }
    catch(const HaltJobException&)
    {
        fprintf(stderr, "gta3sc: decompilation failed\n");
        return EXIT_FAILURE;
    }
}

template<typename OnOutput>
bool decompile(const void* bytecode, size_t bytecode_size,
               const void* script_img, size_t script_img_size,
               ProgramContext& program, Options::Lang lang, OnOutput callback)
{
    Expects(!program.opt.streamed_scripts || script_img != nullptr);

    try
    {
        auto scan_type = program.opt.linear_sweep? Disassembler::Type::LinearSweep :
                                                   Disassembler::Type::RecursiveTraversal;

        auto opt_header = DecompiledScmHeader::from_bytecode(bytecode, bytecode_size,
                                                             program.opt.get_header<DecompiledScmHeader::Version>());
        if(!opt_header)
            program.fatal_error(nocontext, "corrupted scm header");

        DecompiledScmHeader& header = *opt_header;
        size_t ignore_stream_id = -1;

        {
            auto it = std::find_if(header.streamed_scripts.begin(), header.streamed_scripts.end(), [](const auto& pair) {
                return iequal_to()(pair.first, "AAA");
            });
            if(it != header.streamed_scripts.end())
                ignore_stream_id = (it - header.streamed_scripts.begin());
        }

        BinaryFetcher main_segment{ bytecode, std::min<uint32_t>(bytecode_size, header.main_size) };
        std::vector<BinaryFetcher> mission_segments = mission_scripts_fetcher(bytecode, bytecode_size, header, program);
        std::vector<BinaryFetcher> stream_segments;
        if(program.opt.streamed_scripts)
        {
            stream_segments = streamed_scripts_fetcher(script_img, script_img_size, header, program);
        }

        // TODO add more has error in here?
        if(program.has_error())
            throw HaltJobException();

        Disassembler main_segment_asm(program, main_segment, scan_type);
        std::vector<Disassembler> mission_segments_asm;
        std::vector<Disassembler> stream_segments_asm;
        mission_segments_asm.reserve(header.mission_offsets.size());
        stream_segments_asm.reserve(header.streamed_scripts.size());

        // this loop cannot be thread safely unfolded because of main_segment_asm being
        // mutated on all the units.
        for(auto& mission_bytecode : mission_segments)
        {
            mission_segments_asm.emplace_back(program, mission_bytecode, main_segment_asm, scan_type);
            mission_segments_asm.back().run_analyzer();
        }

        // this loop cannot be thread safely unfolded because of stream_segment_asm being
        // mutated on all the units.
        for(size_t i = 0; i < stream_segments.size(); ++i)
        {
            if(i != ignore_stream_id)
            {
                auto& stream_bytecode = stream_segments[i];
                stream_segments_asm.emplace_back(program, stream_bytecode, main_segment_asm, scan_type);
                stream_segments_asm.back().run_analyzer();
            }
        }

        if(true)
        {
            // run main segment analyzer after the missions and streams analyzer
            main_segment_asm.run_analyzer(header.code_offset);
            main_segment_asm.disassembly(header.code_offset);
        }

        for(auto& mission_asm : mission_segments_asm)
            mission_asm.disassembly();

        for(size_t i = 0; i < stream_segments.size(); ++i)
        {
            if(i != ignore_stream_id)
            {
                auto& stream_asm = stream_segments_asm[i];
                stream_asm.disassembly();
            }
        }

        if(lang == Options::Lang::GTA3Script)
        {
            {
                callback("/***** Main Segment *****/");
                DecompilerGTA3Script(program.commands, main_segment_asm.get_data(), 0).decompile(callback);
            }

            for(size_t i = 0; i < mission_segments_asm.size(); ++i)
            {
                auto& mission_asm = mission_segments_asm[i];
                callback(fmt::format("/***** Mission Segment %d *****/", (int)(i)));
                DecompilerGTA3Script(program.commands, mission_asm.get_data(), 1 + i).decompile(callback);
            }

            for(size_t i = 0; i < stream_segments_asm.size(); ++i)
            {
                if(i != ignore_stream_id)
                {
                    auto& stream_asm = stream_segments_asm[i];
                    callback(fmt::format("/***** Streamed Segment %d *****/", (int)(i)));
                    DecompilerGTA3Script(program.commands, stream_asm.get_data(), 1 + i).decompile(callback);
                }
            }
        }
        else if(lang == Options::Lang::IR2)
        {
            {
                std::string temp_string;
                for(size_t i = 0; i < header.models.size(); ++i)
                {
                    temp_string = header.models[i];
                    std::transform(temp_string.begin(), temp_string.end(), temp_string.begin(), ::toupper); // TODO FIXME toupper is bad
                    callback(fmt::format("#DEFINE_MODEL {} -{}", temp_string, i+1));
                }
            }

            auto main_ir2 = DecompilerIR2(program.commands, main_segment_asm.get_data(), 0, main_segment.size, "MAIN", true);
            main_ir2.decompile(callback);

            for(size_t i = 0; i < mission_segments_asm.size(); ++i)
            {
                auto& mission_asm = mission_segments_asm[i];
                auto script_name = fmt::format("MISSION_{}", i);
                callback(fmt::format("#MISSION_BLOCK_START {}", (int)(i)));
                DecompilerIR2(program.commands, mission_asm.get_data(), 0, mission_segments[i].size, std::move(script_name), false, main_ir2).decompile(callback);
                callback("#MISSION_BLOCK_END");
            }

            for(size_t i = 0; i < stream_segments_asm.size(); ++i)
            {
                if(i != ignore_stream_id)
                {
                    auto& stream_asm = stream_segments_asm[i];
                    auto script_name = fmt::format("STREAM_{}", i);
                    callback(fmt::format("#STREAMED_BLOCK_START {}", (int)(i)));
                    DecompilerIR2(program.commands, stream_asm.get_data(), 0, stream_segments[i].size, std::move(script_name), false, main_ir2).decompile(callback);
                    callback("#STREAMED_BLOCK_END");
                }
            }
        }

        if(program.has_error())
            throw HaltJobException();

        return true;
    }
    catch(const HaltJobException&)
    {
        return false;
    }
}


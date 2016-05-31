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

int test_compiler(const GameConfig& gta3_config, const Commands& commands);
int test_decompiler(const GameConfig& gta3_config, const Commands& commands);

int main()
{
    _wchdir(L"../..");

    GameConfig gta3_config;
    gta3_config.has_text_label_prefix = false;
    gta3_config.use_half_float = true;

    Commands commands = Commands::from_xml({ "gta3/constants.xml", "gta3/commands.xml" });

    return test_compiler(gta3_config, commands);
}

int test_compiler(const GameConfig& gta3_config, const Commands& commands)
{
    try {
        std::vector<shared_ptr<Script>> scripts;

        ProgramContext program(gta3_config);

        //const char* input = "intro.sc";
        //const char* input = "test.sc";
        const char* input = "gta3_src/main.sc";

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

        // TODO models
        CompiledScmHeader header(CompiledScmHeader::Version::Liberty, symbols.size_global_vars(), models, main, mission_scripts);

        // not thread-safe
        Expects(gens.size() == scripts.size());
        for(size_t i = 0; i < gens.size(); ++i) // zip
        {
            scripts[i]->size = gens[i].compute_labels();                    // <- maybe???! this is actually thread safe
        }                                                                   //
        Script::compute_script_offsets(scripts, header.compiled_size());    // <- but this isn't


        for(auto& gen : gens)
            gen.generate();

        if(FILE* f = fopen("output.scm", "wb"))
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
        FILE* f = fopen("output.cs", "wb");
        if(f) fclose(f);
    }

    return 0;
}

int test_decompiler(const GameConfig& gta3_config, const Commands& commands)
{
    ProgramContext program(gta3_config);

    auto opt_decomp = Disassembler::from_file(gta3_config, commands, "output.cs");
    if(!opt_decomp)
        program.fatal_error(nocontext, "File {} does not exist", "output.cs");

    Disassembler decomp(std::move(*opt_decomp));
    decomp.run_analyzer();
    auto data = decomp.get_data();

    printf("%d\n", data.size());

    puts(DecompilerContext(commands, std::move(data)).decompile().c_str());

    return 0;
}

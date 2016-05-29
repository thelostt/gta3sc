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

        auto main = Script::create("test.sc"/*"main.sc"*/, ScriptType::Main);
        auto symbols = SymTable::from_script(*main, program);
        symbols.apply_offset_to_vars(2);

        scripts.emplace_back(main);

        auto subdir = main->scan_subdir();

        auto ext_scripts = read_and_scan_symbols(subdir, symbols.extfiles.begin(), symbols.extfiles.end(), ScriptType::MainExtension, program);
        auto sub_scripts = read_and_scan_symbols(subdir, symbols.subscript.begin(), symbols.subscript.end(), ScriptType::Subscript, program);
        auto mission_scripts = read_and_scan_symbols(subdir, symbols.mission.begin(), symbols.mission.end(), ScriptType::Mission, program);

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

        for(auto& x : mission_scripts)
        {
            symbols.merge(std::move(x.second), program);
            scripts.emplace_back(x.first); // maybe move
        }

        symbols.build_script_table(scripts);

        for(auto& script : scripts)
        {
            script->annotate_tree(symbols, commands, program);
        }

        // CompilerContext wants an annotated ASTs, if we have any error, it's possible that
        // the AST is not correctly annotated.
        if(program.has_error())
            throw HaltJobException();

        std::vector<CodeGenerator> gens;
        for(auto& script : scripts)
        {
            CompilerContext cc(script, symbols, commands, program);
            cc.compile();
            gens.emplace_back(std::move(cc), program);
        }

        // Codegen expects a successful compilation.
        if(program.has_error())
            throw HaltJobException();

        // not thread-safe
        Expects(gens.size() == scripts.size());
        for(size_t i = 0; i < gens.size(); ++i) // zip
        {
            scripts[i]->size = gens[i].compute_labels(); // <- maybe???! this is actually thread safe
        }                                                //
        Script::compute_script_offsets(scripts);         // <- but this isn't


        for(auto& gen : gens)
            gen.generate();

        if(FILE* f = fopen("output.cs", "wb"))
        {
            auto guard = make_scope_guard([&] {
                fclose(f);
            });

            for(auto& gen : gens)
            {
                write_file(f, gen.bytecode.get(), gen.script->size.value());
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
